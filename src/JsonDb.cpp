/*
 * JsonDb.cpp
 *
 *  Created on: May 18, 2017
 *      Author: daniel
 */

#include "JsonDb.h"

#include <elladan/Exception.h>
#include <elladan/FileManipulator.h>
#include <initializer_list>
#include <memory>
#include <utility>
#include <cassert>

#include "Clause.h"
#include "TransactionReadFile.h"
#include "TransactionWriteFile.h"

namespace elladan {
namespace jsondb {

JsonDb::JsonDb() {
   _basepath = "./data/";
   _format = json::StreamFormat::BSON;
   _decodingOption = json::DecodingFlags::DF_ALLOW_NULL | json::DecodingFlags::DF_ALLOW_COMMA_ERR;
}

void JsonDb::setBasePath(const std::string& basePath) {
   _basepath = removeDoubleSlash(basePath + "/");
   createFolder(_basepath);
}

void JsonDb::setDecodingOption(json::DecodingOption decodingFlags) {
   _decodingOption = decodingFlags;
}

void JsonDb::setFormat(json::StreamFormat format) {
   _format = format;
}

void JsonDb::addIndex(const std::string& clas, const std::string& path, bool unique, bool caseSensitive) {
   DEBUG("Add idx %s to %s", path.c_str(), clas.c_str());
   auto& clasPaths = _indexes[clas];

   auto ite = clasPaths.find(path);

   // FIXME: allow change of index parameters.
   if (ite != clasPaths.end()) {
      DEBUG("Already defined %s", path.c_str());
      throw Exception("Index " + path + " of class " + clas + " is already defined");
   }

   clasPaths[path] = std::make_shared<Index>(clas, path, unique, caseSensitive);
}

void JsonDb::setId(const std::string& clas, json::Json_t defaultValue, bool caseSensitive) {
   DEBUG("Add id to %s", clas.c_str());
   auto& clasPaths = _indexes[clas];

   // FIXME: allow change of index parameters.
   auto ite = clasPaths.find(ID_PATH);
   if (ite != clasPaths.end()) {
      DEBUG("Already defined %s", path.c_str());
      throw Exception("Id of class " + clas + " is already defined");
   }

   clasPaths[ID_PATH] = std::make_shared<IdIndex>(clas, caseSensitive, defaultValue);
}

void JsonDb::scanIndex() {
   // FIXME: make thread safe

   // For each class
   for (auto& clasIte : _indexes) {
      DEBUG("Loading idx of class %s", clasIte.first.c_str());

      // Clear all of its idx.
      for (auto& pathIte : clasIte.second)
         pathIte.second->clear();

      // Reload every file.
      for (auto& ite : listFile(_basepath + clasIte.first, ".bson")) {
         DEBUG("Loading idx of element %s", ite.c_str());
         TransactionReadFile file(clasIte.first + "/" + ite, *this);

         if (!file) {
            DEBUG("Could not open %s\n", file.getFileName().c_str());
            continue;
         }

         json::Json_t ele = json::Json::read(&file.getFile(), _decodingOption, json::StreamFormat::BSON);

         auto ids = json::Json::getChild(ele, ID_PATH);
         if (ids.size() != 1)
            throw Exception("No id on indexed element " + ite);

         elladan::json::Json_t id = ids.front();
         if (id->getType() == json::JSON_NONE || id->getType() == json::JSON_NULL)
            throw Exception("invalid empty id on indexed element " + ite);

         // For all path
         for (auto& pathIte : clasIte.second) {
            Index* idx = pathIte.second.get();

            // Get the values.
            auto values = json::Json::getChild(ele, pathIte.first);

            // If there is none, still add it to the index.
            if (values.empty())
               values.emplace_back(json::Json_t());

            for (auto aa : values) {
               IndexElement val = {id, aa};
               idx->add(val);
            }
         }
      }

      // Copy and paste from above, for json files.
      for (auto& ite : listFile(_basepath + clasIte.first, ".json")) {
         DEBUG("Loading idx of element %s", ite.c_str());
         TransactionReadFile file(clasIte.first + ite, *this);
         json::Json_t ele = json::Json::read(&file.getFile(), _decodingOption, json::StreamFormat::JSON);
         json::Json_t id = json::Json::getChild(ele, ID_PATH).front();

         // For all path
         for (auto& pathIte : clasIte.second) {
            Index* idx = pathIte.second.get();

            // Get the values.
            auto values = json::Json::getChild(ele, pathIte.first);
            for (auto aa : values)
               idx->add(IndexElement(id, aa));

            // If there is none, still add it to the index.
            if (values.empty())
               idx->add(IndexElement(id, json::Json_t()));
         }
      }

      DEBUG("We have found %li elements of type %s", clasIte.second.at(ID_PATH).getElements().size(), clasIte.first.c_str());
   }
}

void JsonDb::save(const std::string& clas, json::Json_t id, json::Json_t ele) {
   DEBUG("Saving object %s", to_string(id).c_str());
   std::string path = _basepath + removeDoubleSlash(clas + "/");
   createFolder(path);

   // Save file.
   static const std::string format[2] = { ".json", ".bson" };

   TransactionWriteFile file(path + to_string(id) + format[_format == json::StreamFormat::BSON], *this);
   try {
      ele->write(&file.getFile(), json::EncodingFlags(), _format);
      DEBUG("Saved object %s", to_string(id).c_str());
   } catch (std::exception& e) {
      DEBUG("Error saving object %s", to_string(id).c_str());
      file.rollbackAndClose();
      throw e;
   }

   // Update index
   for (auto& ite : _indexes[clas]) {
      DEBUG("Updating index  %s", ite.first.c_str());
      Index* idx = ite.second.get();
      // Clear previous index
      idx->removeAll(id);

      // Add the new values.
      IndexElement val;
      val._uid = id;
      auto values = ele->getChild(ele, ite.first);
      for (auto aa : values) {
         val._value = aa;
         idx->add(val);
      }

      if (values.empty()) {
         //            val._value = json::Json_t();
         idx->add(val);
      }
   }
}

size_t JsonDb::count(const std::string& clas) {
   auto classIdx = _indexes.find(clas);
   if (classIdx != _indexes.end())
      return classIdx->second.count(ID_PATH);
   return 0;
}
size_t JsonDb::countConditionnal(const std::string& clas, const jsondb::Clause& where) {
   size_t retVal;

   JsonTSet idSet;
   auto classIdx = _indexes.find(clas);
   if (classIdx != _indexes.end()) {
      auto idx = classIdx->second.find(ID_PATH);
      if (idx != classIdx->second.end()) {
         for (auto ite : idx->second->getElements())
            idSet.insert(ite.first);
      }
   }

   if (!idSet.empty())
      where.filter(idSet, clas, *this);

   return idSet.size();
}



json::Json_t JsonDb::load(const std::string& clas, json::Json_t id) {
   std::string fileName = clas + "/" + to_string(id);
   TransactionReadFile file(fileName + ".bson", *this);
   json::StreamFormat format = json::StreamFormat::BSON;
   if (!file) {
      file.open(fileName + ".json");
      format = json::StreamFormat::JSON;
   }
   if (!file.getFile().is_open())
      return std::make_shared<json::Json>();

   return json::Json::read(&file.getFile(), _decodingOption, format);
}

std::vector<json::Json_t> JsonDb::loadAll(std::string clas, Sort sort) {
   std::vector<json::Json_t> retVal;
   clas = removeDoubleSlash(clas + "/");

   for (auto& ite : listFile(_basepath + clas, ".bson")) {
      TransactionReadFile file(clas + ite, *this);
      retVal.push_back(json::Json::read(&file.getFile(), _decodingOption, json::StreamFormat::BSON));
   }

   for (auto& ite : listFile(_basepath + clas, ".json")) {
      TransactionReadFile file(clas + ite, *this);
      retVal.push_back(json::Json::read(&file.getFile(), _decodingOption, json::StreamFormat::JSON));
   }

   sort.doSort(retVal);

   return retVal;
}

std::vector<json::Json_t> JsonDb::extract(const std::string& clas, json::Json_t id, const std::string& path) {
   TransactionReadFile file(clas + "/" + to_string(id) + ".bson", *this);
   json::StreamFormat format = json::StreamFormat::BSON;
   if (!file) {
      file.open(_basepath + clas + "/" + to_string(id) + ".json");
      format = json::StreamFormat::JSON;
   }
   if (!file)
      return std::vector<json::Json_t>();

   return json::Json::extract(&file.getFile(), _decodingOption, format, path);
}

std::vector<json::Json_t> JsonDb::loadConditionnal(const std::string& clas, const Clause& where, Sort sort) {
   std::vector<json::Json_t> retVal;

   JsonTSet idSet;
   auto classIdx = _indexes.find(clas);
   if (classIdx != _indexes.end()) {
      auto idx = classIdx->second.find(ID_PATH);
      if (idx != classIdx->second.end()) {
         for (auto ite : idx->second->getElements())
            idSet.insert(ite.first);
      }
   }

   if (idSet.empty())
      return retVal;

   where.filter(idSet, clas, *this);

   for (auto ite : idSet)
      retVal.push_back(load(clas, ite));

   sort.doSort(retVal);

   return retVal;
}

void JsonDb::remove(const std::string& clas, json::Json_t id) {
   std::string filename = clas + "/" + to_string(id);
   ::remove((filename+ ".bson").c_str());
   ::remove((filename+ ".json").c_str());
}

void JsonDb::remove(const std::string& clas, const jsondb::Clause& where) {
   std::vector<json::Json_t> retVal;

   JsonTSet idSet;
   auto classIdx = _indexes.find(clas);
   if (classIdx != _indexes.end()) {
      auto idx = classIdx->second.find(ID_PATH);
      if (idx != classIdx->second.end()) {
         for (auto ite : idx->second->getElements())
            idSet.insert(ite.first);
      }
   }

   if (idSet.empty())
      return;

   where.filter(idSet, clas, *this);

   for (auto ite : idSet)
      remove(clas, ite);
}


std::shared_ptr<IdIndex>  JsonDb::getIdGen(const std::string& clas){
   auto clasIdx = _indexes.find(clas);
   if (clasIdx == _indexes.end())
      throw Exception((std::string("Require to gen id but class ") + clas) + " is not part of db index");

   auto& idx = clasIdx->second;
   auto idIdx = idx.find(ID_PATH);
   if (idIdx == idx.end())
      throw Exception((std::string("Require to gen id but class ") + clas) + " has no indexed \"id\" field");

   return std::dynamic_pointer_cast<IdIndex>(idIdx->second);
}

json::Json_t JsonDb::genId(const std::string& clas){
   std::shared_ptr<IdIndex> ptr = getIdGen(clas);
   assert(ptr.get() != nullptr);
   return ptr->getNextLogical();
}


void JsonDb::getLock(const std::string& filename) {
   std::unique_lock<std::mutex> guard(_fileMutex);

   while (_fileLock.count(filename)) {
      _fileCond.wait(guard);
   }
   _fileLock.insert(filename);
}

void JsonDb::releaseLock(const std::string& filename) {
   do{
      std::lock_guard<std::mutex> guard(_fileMutex);
      _fileLock.erase(filename);
   } while(0);
   _fileCond.notify_all();
}

}
} // namespace elladan::jsondb

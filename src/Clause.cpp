/*
 * Clause.cpp
 *
 *  Created on: May 23, 2017
 *      Author: daniel
 */

#include "Clause.h"

#include <elladan/Exception.h>
#include <elladan/Set.h>
#include <memory>
#include <utility>

#include "Index.h"
#include "JsonDb.h"

using namespace elladan::json;
using std::to_string;

namespace elladan {
namespace jsondb {





//AndClause::AndClause() : Clause() {
//}
//AndClause* AndClause::add(std::shared_ptr<Clause>&& c) {
//   _clauses.emplace_back(c);
//   return this;
//}
bool AndClause::filter(JsonObject_t root) const {
   for (auto& ite : _clauses)
      if (!ite.filter(root))
         return false;
   return true;
}
void AndClause::filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const {
   for (auto& ite : _clauses) {
      ite.filter(resultSet, clas, db);
      if (resultSet.empty()) return;
   }
}
//
//OrClause::OrClause() : Clause() {
//}
//OrClause* OrClause::add(std::shared_ptr<Clause>&& c) {
//   _clauses.emplace_back(c);
//   return this;
//}
bool OrClause::filter(JsonObject_t root) const {
   for (auto& ite : _clauses)
      if (ite.filter(root)) return true;
   return false;
}
void OrClause::filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const {
   JsonTSet testSet;
   JsonTSet goodSet;
   for (auto& ite : _clauses) {
      // Test with a copy.
      testSet = resultSet;
      ite.filter(testSet, clas, db);

      // Keep track of all valid id.
      goodSet.insert(testSet.begin(), testSet.end());

      // Shorten search by checking only those id that are not already good.
      elladan::keep_difference(resultSet, testSet);

      if (resultSet.empty()) break;
   }
   resultSet = goodSet;
}
//
//
//class CmpClause: public Clause
//{
//
//public:
//   enum Mode
//   {
//      NE, LT, LE, EQ, GE, GT
//   };
//
//   CmpClause(const std::string& path, Mode mode, json::Json_t value, bool caseSensitive = true);
//   bool testRelation(json::Json_t value) const;
//   bool filter(json::JsonObject_t root) const;
//   void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const;
//
//protected:
//   json::Json_t _expected;
//   std::string _path;
//   Mode _mode;
//   bool _caseSensitive;
//};
//
CmpClause::CmpClause(const std::string& path, Mode mode, Json_t value, bool caseSensitive) 
: _path(path), _expected(value), _mode(mode), _caseSensitive(caseSensitive)
{
   if (_path == ID && value->getType() != JSON_STRING) 
      _expected = std::make_shared<JsonString>(to_string(value));

   if (!caseSensitive && _expected->getType() == JSON_STRING) 
      _expected->toString()->value = toLower(_expected->toString()->value);
}

bool CmpClause::filter(JsonObject_t root) const {
   auto matchingElements = root->getChild(root, _path);

   // Exception: the only one where if any match the value, is not good. All other are good if any match it.
   // FIXME : no case sensitive,
   if (_mode == NE) {
      for (auto ite : matchingElements) {
         // FIXME : mismatch if not equal?
         if (ite->getType() != _expected->getType())
            throw Exception("Element " + _path + " of " + to_string(root->getChild(root, ID).front()) + " is of the wrong type.");

         if (ite != _expected)
            continue;

         return false;
      }

      return true;
   }
   else {
      for (auto ite : matchingElements) {
         // Test if the element type are the same.
         // IMP: allow bool -> int -> float comparison.
         if (ite->getType() != _expected->getType())
            throw Exception("Element " + _path + " of " + to_string(root->getChild(root, ID).front()) + " is of the wrong type.");

         if (!testRelation(ite))
            continue;

         return true;

      }
      return false;
   }
}
bool CmpClause::testRelation(Json_t value) const {
   if (!_caseSensitive && value->getType() == JSON_STRING)
      value = std::make_shared<JsonString>(toLower(value->toString()->value));

   int cmp = value->cmp(_expected.get());

   switch (_mode) {
      case NE:   return cmp != 0;
      case EQ:   return cmp == 0;
      case LT:   return cmp <  0;
      case LE:   return cmp <= 0;
      case GE:   return cmp >= 0;
      case GT:   return cmp >  0;
      default:   return false;
   }
}
void CmpClause::filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const {
   JsonTSet goodSet;

   bool haveIdx = false;

   auto classIdx = db.getIndex().find(clas);
   if (classIdx != db.getIndex().end()) {
      auto idxIte = classIdx->second.find(_path);

      // We have an index.
      if (idxIte != classIdx->second.end()) {
         for (auto docIte : idxIte->second->getElements()) {
            if (testRelation(docIte.first) ^ (_mode == NE))
               goodSet.insert(docIte.second._uid);
         }
         haveIdx = true;
      }
   }

   // We don't have an index.
   if (!haveIdx) {
      for (auto docIte : resultSet) {
         bool found = false;
         for (auto extIte : db.extract(clas, docIte, _path)) {
            if (testRelation(extIte) ^ (_mode == NE)){
               goodSet.insert(docIte);
               break;
            }
         }
      }
   }

   if (_mode == NE)
      keep_difference(resultSet, goodSet);
   else
      keep_intersect(resultSet, goodSet);
}
//
//class ExistClause: public Clause
//{
//public:
//   ExistClause(const std::string& path);
//   bool filter(json::JsonObject_t root) const ;
//   void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const;
//protected:
//   std::string _path;
//};
//
ExistClause::ExistClause(const std::string& path) : _path(path) {}
bool ExistClause::filter(json::JsonObject_t root) const {
   return !root->getChild(root, _path).empty();
}

void ExistClause::filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const{

   JsonTSet goodSet;

   bool haveIdx = false;

   auto classIdx = db.getIndex().find(clas);
   if (classIdx != db.getIndex().end()) {
      auto idxIte = classIdx->second.find(_path);

      // We have an index.
      if (idxIte != classIdx->second.end()){
         for (auto docIte : idxIte->second->getElements()){
            goodSet.insert(docIte.second._uid);
         }
      }
   }

   // We don't have an index.
   if (!haveIdx) {
      for (auto docIte : resultSet) {
         bool found = false;
         for (auto extIte : db.extract(clas, docIte, _path)) {
            goodSet.insert(docIte);
            break;
         }
      }
   }
   resultSet = goodSet;
}
//
//std::shared_ptr<AndClause> Clause::And()   { return std::make_shared<AndClause  >(); }
//std::shared_ptr<OrClause>  Clause::Or()    { return std::make_shared<OrClause   >(); }
//std::shared_ptr<Clause> Clause::Exist(const std::string& path) { return std::make_shared<ExistClause>(path); }
//std::shared_ptr<Clause> Clause::CmpLT(const std::string& path, Json_t value, bool caseSensitive) { return std::make_shared<CmpClause>(path, CmpClause::LT, value, caseSensitive); }
//std::shared_ptr<Clause> Clause::CmpLE(const std::string& path, Json_t value, bool caseSensitive) { return std::make_shared<CmpClause>(path, CmpClause::LE, value, caseSensitive); }
//std::shared_ptr<Clause> Clause::CmpEQ(const std::string& path, Json_t value, bool caseSensitive) { return std::make_shared<CmpClause>(path, CmpClause::EQ, value, caseSensitive); }
//std::shared_ptr<Clause> Clause::CmpNE(const std::string& path, Json_t value, bool caseSensitive) { return std::make_shared<CmpClause>(path, CmpClause::NE, value, caseSensitive); }
//std::shared_ptr<Clause> Clause::CmpGE(const std::string& path, Json_t value, bool caseSensitive) { return std::make_shared<CmpClause>(path, CmpClause::GE, value, caseSensitive); }
//std::shared_ptr<Clause> Clause::CmpGT(const std::string& path, Json_t value, bool caseSensitive) { return std::make_shared<CmpClause>(path, CmpClause::GT, value, caseSensitive); }


} // namespace jsondb
} // namespace elladan

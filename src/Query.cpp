#include "Query.h"

#include <elladan/VMap.h>
#include <algorithm>
#include <fstream>
#include <iterator>

#include "JsonDb.h"

namespace elladan {
namespace jsondb {

//template<typename T>
Query::Query (const std::string& className, const JsonDb& db)
: _db(db)
, _className(className)
{ }

const std::vector<json::Json>& Query::loadAll() {
   getLoadResult();
   return _resultIds.value();
}

Query& Query::operator++() {
   if (_resultIds.has_value() && _resIte != _resultIds.value().end())
      ++_resIte;
   return *this;
}

Query& Query::operator--() {
   if (_resultIds.has_value() && _resIte != _resultIds.value().begin())
      --_resIte;
   return *this;
}

Query& Query::resetLoad() {
   if (_resultIds.has_value())
      _resIte = _resultIds.value().begin();
   return *this;
}

json::Json* Query::getLoadResult() const {
   if (!_resultIds.has_value()) {

      _resultIds = std::vector<json::Json>();
      std::vector<json::Json>& resIdx = _resultIds.value();

      // Get File
      std::ifstream file(_db.config.basePath + '/' + _className);
      if (file.is_open()) {
         if (!_id.isOfType<json::Null>())
            resIdx = _db.config.extract(file, std::to_string(_id));
         else {
            json::Json fromFile = _db.config.read(file);
            for (auto& ite : fromFile.as<json::Object>()) {
               if (_clause.filter(ite.first, ite.second))
                  resIdx.push_back(ite.second);
            }
         }
      }

      _sort.sort(resIdx);
      _resIte = resIdx.begin();
   }

   if (_resIte == _resultIds.value().end())
      return nullptr;

   return &*_resIte;
}

void Query::resetResult() const {
   _resultIds = std::nullopt;
}

Query& Query::id(const json::Json& id) {
   _id = id;
   resetLoad();
   return *this;
}

Query& Query::where(const Clause& clause) {
   _clause = clause;
   resetLoad();
   return *this;
}

Query& Query::sortBy(const Sort& sort) {
   _sort = sort;
   resetLoad();
   return *this;
}

Query& Query::limit(int limit) {
   _limit = limit;
   resetLoad();
   return *this;
}

Query& Query::offset(int offset) {
   _offset = offset;
   resetLoad();
   return *this;
}

unsigned Query::size() const {
   return _resultIds.has_value() ? _resultIds.value().size() : 0;
}

json::Json* Query::load() {
   return getLoadResult();
}
json::Json& Query::get() const {
   getLoadResult();
   return *_resIte;
}
bool Query::hasResult() const {
   return !!getLoadResult();
}
bool Query::operator!() const {
   return !getLoadResult();
}

} } // namespace elladan::jsondb

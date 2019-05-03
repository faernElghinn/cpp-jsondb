#pragma once

#include <optional>
#include <string>
#include <vector>
#include <elladan/json/json.h>

#include "Clause.h"
#include "Sort.h"

namespace elladan {
namespace jsondb {
struct Sort;
} /* namespace jsondb */
} /* namespace elladan */

namespace elladan {
namespace jsondb {

struct JsonDb;

//template<typename T>
struct Query {
   Query (const std::string& className, const JsonDb& db);
   Query(const Query&) = delete;
   Query& operator=(const Query&) = delete;
   Query(Query&&) = default;
   Query& operator=(Query&&) = default;
   ~Query() = default;

   const std::string _className;
   const JsonDb& _db;

   static const json::Json NoId;
   Query& id(const json::Json& id);
   json::Json _id = NoId;

   static const Clause NoClause;
   Clause _clause = NoClause;
   Query& where(const Clause& clause);

   static const Sort NoSort;
   Sort _sort = NoSort;
   Query& sortBy(const Sort& sort);

   unsigned _limit = -1U;
   Query& limit(int limit);

   unsigned _offset = 0;
   Query& offset(int offset);

   unsigned size() const;
   json::Json* load();
   const std::vector<json::Json>& loadAll();
   json::Json& get()  const;
   bool hasResult() const;
   bool operator!() const;
   Query& operator++();
   Query& operator--();
   Query& resetLoad();

protected:
   mutable std::optional<std::vector<json::Json>> _resultIds;
   mutable std::vector<json::Json>::iterator _resIte;
   json::Json* getLoadResult() const;
   void resetResult() const;

};


} } // namespace elladan::jsondb

#pragma once

#include <elladan/json/json.h>
#include <elladan/Stringify.h>
#include <algorithm>
#include <string>
#include <vector>

namespace elladan {
namespace jsondb {


struct Sort {
   enum Modifer {
      DEFAULT = 0,
      IGNORE_CASE = 1,
      MIN = 2,
      MAX = 4
   };
   struct Ele{
      std::string path;
      bool asc;
      Modifer mod;
   };
   std::vector<Ele> pathes;

   void sort(std::vector<json::Json>& valueToSort) const;

protected:
   const json::Json* getSingle(const Ele& p, const json::Json& json, json::Json& keep) const;
   static json::Json NoValue;
   mutable json::Json lhsTmp;
   mutable json::Json rhsTmp;
};



} } // namespace elladan::jsondb

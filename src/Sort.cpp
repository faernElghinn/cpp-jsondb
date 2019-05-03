#include "Sort.h"

#include <elladan/json/json.h>
#include <elladan/Stringify.h>
#include <algorithm>
#include <string>
#include <vector>

namespace elladan {
namespace jsondb {

const json::Json* Sort::getSingle(const Ele& p, const json::Json& json, json::Json& keep) const {
   const json::Json* e = &NoValue;
   json::Json tmp;
   auto list = json.get(p.path);
   if (list.size() >= 1) {
      e = list.front();
      if (list.size() > 1) {
         switch (p.mod) {
            case MAX:
               for (int i = 1; i < list.size(); ++i) {
                  auto o = list[i];
                  if (p.mod & IGNORE_CASE) {
                     if (const std::string* c = o->cast<std::string>()) {
                        tmp = elladan::toLower(*c);
                        o = &tmp;
                     }
                  }
                  if (*e < *o) {
                     if (o == &tmp) {
                        keep = tmp;
                        o = &keep;
                     }
                     e = o;
                  }
               }
               break;

            case MIN:
               for (int i = 1; i < list.size(); ++i) {
                  auto o = list[i];
                  if (p.mod & IGNORE_CASE) {
                     if (const std::string* c = o->cast<std::string>()) {
                        tmp = elladan::toLower(*c);
                        o = &tmp;
                     }
                  }
                  if (*e > *o) {
                     if (o == &tmp) {
                        keep = tmp;
                        o = &keep;
                     }
                     e = o;
                  }
               }
               break;
         }
      }
   }
   return e;
}

void Sort::sort(std::vector<json::Json>& valueToSort) const {
   std::stable_sort(valueToSort.begin(), valueToSort.end(), [&](const json::Json& lhs, const json::Json& rhs) -> bool {
      for (auto& p : pathes) {
         auto l = getSingle(p, lhs, lhsTmp);
         auto r = getSingle(p, rhs, rhsTmp);

         if (l < r) return p.asc;
         if (r < l) return !p.asc;
      }
      return false;
   });
}

}
} // namespace elladan::jsondb

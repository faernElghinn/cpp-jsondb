#include "Clause.h"

#include <elladan/Exception.h>
#include <elladan/Stringify.h>
#include <elladan/json/json.h>

using namespace elladan::json;

namespace elladan {
namespace jsondb {

Clause::Clause(const Relative& left, Type t, const Relative& right) {
   if (!(NONE < t && t < LEFT_RIGHT_CMP))
      throw elladan::Exception("Invalid use of clause");
   type = t;
   lhs.push_back(left);
   rhs.push_back(right);
}
Clause::Clause(Type t, std::initializer_list<Relative> list) {
   if (!(LEFT_RIGHT_CMP < t && t < CLAUSE_MAX))
      throw elladan::Exception("Invalid use of clause");
   type = t;
   lhs = list;
}

const json::Json& Clause::get(const Relative& value, const Json& json, Json& tmp) const {
   switch (value.type) {
      case Relative::JSON:
         return value.json;

      case Relative::STR:{
         auto ele = json.get(value.str);
         if (ele.empty())
            return tmp = Json();
         else if (ele.size() > 1)
            throw Exception("Does not support query with more than result");
         else
            return tmp = ele.front();
      } break;

      case Relative::CLAUSE:{
         auto clause = value.cla;

         switch (clause.type) {
            case MAX: {
               const Json* max = nullptr;
               for (auto ite : clause.lhs) {
                  const Json& i = clause.get(ite, json, clause.lhsTmp);
                  if (!max || *max < i)
                     max = &i;
               }

               if (!max)
                  return tmp = Json();

               return *max;
            } break;

            case MIN: {
               const Json* min = nullptr;
               for (auto ite : clause.lhs) {
                  const Json& i = clause.get(ite, json, clause.lhsTmp);
                  if (!min || *min > i)
                     min = &i;
               }

               if (!min)
                  return tmp = Json();

               return *min;
            } break;

            case UPPERCASE: {
               if (clause.lhs.size() != 1)
                  throw Exception("Expect one element");
               tmp = clause.get(clause.lhs.front(), json, clause.lhsTmp);
               if (std::string* c = tmp.cast<std::string>())
                  elladan::toUpper(*c);

               return tmp;

            } break;

            case LOWERCASE: {
               if (clause.lhs.size() != 1)
                  throw Exception("Expect one element");
               tmp = clause.get(clause.lhs.front(), json, clause.lhsTmp);
               if (std::string* c = tmp.cast<std::string>())
                  elladan::toLower(*c);

               return tmp;
            } break;

            default:
               throw Exception("Getting a single value from a comparator");
               break;
         }
      } break;

      default:
         throw Exception("Unsupported Relative type. Most likely something was added but not everywhere");
   }

   return tmp = Json();
}

//const Json& Clause::getOneJson(const std::vector<Relative> toExtractToOne, const Json& element, Json& tmp) const {
//   if (toExtractToOne.empty())
//      return tmp = Json();
//
//   if (toExtractToOne.size() != 1)
//      throw Exception("Expect one element");
//
//   return get(toExtractToOne.front(), element, tmp);
//}

bool Clause::filter(const std::string& id, const Json& element) const {
   switch (type) {
      case NONE: return true;

      case LT:   return getOneJson(lhs, element, lhsTmp) <  getOneJson(rhs, element, rhsTmp);
      case LTE:  return getOneJson(lhs, element, lhsTmp) <= getOneJson(rhs, element, rhsTmp);
      case EQ:   return getOneJson(lhs, element, lhsTmp) == getOneJson(rhs, element, rhsTmp);
      case NE:   return getOneJson(lhs, element, lhsTmp) != getOneJson(rhs, element, rhsTmp);
      case GTE:  return getOneJson(lhs, element, lhsTmp) >= getOneJson(rhs, element, rhsTmp);
      case GT:   return getOneJson(lhs, element, lhsTmp) >  getOneJson(rhs, element, rhsTmp);

      case IN: {
         auto l = getOneJson(lhs, element, lhsTmp);
         for (auto& ite : rhs) {
            if (l == get(ite, element, rhsTmp))
               return true;
         }
         return false;
      } break;

      case NOT_IN: {
         auto l = getOneJson(lhs, element, lhsTmp);
         for (auto& ite : rhs) {
            if (l == get(ite, element, rhsTmp))
               return false;
         }
         return true;
      } break;

      case OR: {
         for (auto& ite : lhs) {
            if (ite.cla.filter(id, element))
               return true;
         }
         return false;
      } break;

      case AND: {
         for (auto& ite : lhs) {
            if (!ite.cla.filter(id, element))
               return false;
         }
         return true;
      } break;

      case NOT: {
         if (lhs.size() != 1)
            throw Exception("Not expect one element");

         if (lhs.front().type != Relative::CLAUSE)
            throw Exception("Not expect a subclause");

         return !lhs.front().cla.filter(id, element);
      } break;

      default:
         return false;
   }
}



} } // namespace elladan::jsondb

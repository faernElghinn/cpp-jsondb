#pragma once

#include <elladan/json/json.h>
#include <initializer_list>
#include <string>
#include <vector>

namespace elladan {
namespace jsondb {

struct Relative;

struct Clause {
   enum Type {
      NONE,    //< Always true.

      // Node compare.
      LT,      //< Less than
      LTE,     //< Less than equals
      EQ,      //< Equals
      NE,      //< Not Equals
      GTE,     //< Greater than equals
      GT,      //< Greater than

      LEFT_RIGHT_CMP,

      IN,      //< Within the target value
      NOT_IN,  //< Without the target value

      OR,      //< Must match any condition.
      AND,     //< Must match all condition.
      NOT,     //< Invert selected clause.

      // Object / list specific
//      INTERSECT,     //<
//      EXIST,         //<

      // Number modifier
      MAX,        //< Maximum of list, as defined with the > operator.
      MIN,        //< Minimum of list, as defined with the < operator.

      // Text modifier
      UPPERCASE,  //< std::toUpper(std::to_string(value))
      LOWERCASE,  //< std::toLower(std::to_string(value))

      CLAUSE_MAX,
   };

   Clause() = default;
   Clause(const Relative& left, Type type, const Relative& cmp);
   Clause(Type, std::initializer_list<Relative>);
   ~Clause() = default;
   Clause(const Clause&) = default;
   Clause(Clause&&) = default;
   Clause& operator=(const Clause&) = default;
   Clause& operator=(Clause&&) = default;

   const json::Json& get(const Relative& value, const json::Json& json, json::Json& tmp) const;
   const json::Json& getOneJson(const std::vector<Relative> toExtractToOne, const json::Json& element, json::Json& tmp) const;
   bool filter(const std::string& id, const json::Json& element) const;

protected:
   Type type = NONE;
   std::vector<Relative> lhs;
   std::vector<Relative> rhs;

   mutable  json::Json lhsTmp;
   mutable  json::Json rhsTmp;
};

struct Relative {
   json::Json json;
   std::string str;
   Clause cla;
   enum Val { NO_VALUE, JSON, STR, CLAUSE } type;
   Relative() : type(NO_VALUE) { }
   Relative(const json::Json& js)  : type(JSON)   { this->json = js;    }
   Relative(const std::string& st) : type(STR)    { this->str = st;     }
   Relative(const Clause& clause)  : type(CLAUSE) { this->cla = clause; }
};



} } // namespace elladan::jsondb

/*
 * Clause.h
 *
 *  Created on: May 23, 2017
 *      Author: daniel
 */

#pragma once

#include <elladan/json/json.h>
#include <set>
#include <string>
#include <vector>

namespace elladan {
namespace jsondb {

class JsonDb;
class AndClause;
class OrClause;
class CmpClause;
class ExistClause;

typedef std::set<json::Json_t, json::cmpJson> JsonTSet;

class Clause {
public:
   Clause() = default;

   template <typename T>
   Clause(T x) : _self(new model<T>(std::move(x))) {}

   Clause(const Clause& x) : _self(x._self->copy()) {}
   Clause(Clause&& x) noexcept = default;

   Clause& operator=(const Clause& x) { Clause tmp(x); *this = std::move (tmp); return *this; }
   Clause& operator=(Clause&& x) noexcept = default;

   void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const
   { if (_self.get()) _self->filter(resultSet, clas, db); }

   bool filter(json::JsonObject_t root) const
   { return !_self.get() || _self->filter(root); }

private:
   struct concept_t {
      virtual ~concept_t() = default;
      virtual concept_t* copy() const = 0;
      virtual void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const = 0;
      virtual bool filter(json::JsonObject_t root) const = 0;
   };

   template <typename T>
   struct model : concept_t {
      model(T x) : _data(std::move(x)) {}
      concept_t* copy() const {return new model(*this); }
      void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const {
        _data.filter(resultSet, clas, db);
      }
      bool filter(json::JsonObject_t root) const {
        return _data.filter(root);
      }
      T _data;
   };

   std::unique_ptr<concept_t> _self;
};

class AndClause
{
public:
    AndClause() = default;
    template <typename T>
    AndClause* add(T&& c){
       _clauses.emplace_back(Clause(c));
       return this;
    }

    bool filter(json::JsonObject_t root) const ;
    void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const;
protected:
    std::vector<Clause> _clauses;
};

class OrClause
{
public:
    OrClause() = default;
    template <typename T>
    OrClause* add(T&& c){
       _clauses.emplace_back(Clause(c));
       return this;
    }
    bool filter(json::JsonObject_t root) const;
    void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const;
protected:
    std::vector<Clause> _clauses;
};


class CmpClause
{
public:
   enum Mode
   {
      NE, LT, LE, EQ, GE, GT
   };

   template <typename T>
   CmpClause(Mode mode, const std::string& path, const T& value, bool caseSensitive = true) 
   : CmpClause(path, mode, elladan::json::toJson(value), caseSensitive)
   { }

   CmpClause(const std::string& path, Mode mode, json::Json_t value, bool caseSensitive = true);
   bool testRelation(json::Json_t value) const;
   bool filter(json::JsonObject_t root) const;
   void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const;

protected:
   json::Json_t _expected;
   std::string _path;
   Mode _mode;
   bool _caseSensitive;
};

class ExistClause
{
public:
   ExistClause(const std::string& path);
   bool filter(json::JsonObject_t root) const;
   void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const;
protected:
   std::string _path;
};



//class Clause
//{
//protected:
//    Clause(){}
//    virtual ~Clause(){}
//public:
//    static std::shared_ptr<AndClause> And();
//    static std::shared_ptr<OrClause> Or();
//    static std::shared_ptr<Clause> Exist(const std::string& path);
//    static std::shared_ptr<Clause> CmpLT(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
//    static std::shared_ptr<Clause> CmpLE(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
//    static std::shared_ptr<Clause> CmpEQ(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
//    static std::shared_ptr<Clause> CmpNE(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
//    static std::shared_ptr<Clause> CmpGE(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
//    static std::shared_ptr<Clause> CmpGT(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
//
//    // Return true if the element match the filter condition.
//    virtual bool filter(json::JsonObject_t root) const = 0;
//    virtual void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const = 0;
//};
//
//class AndClause: public Clause
//{
//public:
//    AndClause();
//    AndClause* add(std::shared_ptr<Clause>&& c);
//    bool filter(json::JsonObject_t root) const ;
//    void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const;
//protected:
//    std::vector<std::shared_ptr<Clause> > _clauses;
//};
//
//class OrClause: public Clause
//{
//public:
//    OrClause();
//    OrClause* add(std::shared_ptr<Clause>&& c);
//    bool filter(json::JsonObject_t root) const;
//    void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const;
//protected:
//    std::vector<std::shared_ptr<Clause> > _clauses;
//};

} } // namespace elladan::jsondb

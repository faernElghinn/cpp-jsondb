/*
 * Controller.h

 *
 *  Created on: Dec 21, 2016
 *      Author: daniel
 */

#pragma once

#include <elladan/json/json.h>
#include <elladan/VMap.h>
#include <stddef.h>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

#include "../JsonDb.h"
#include "Action.h"
#include "ReadStatement.h"

namespace elladan {
namespace jsondb {

/// This is the main interface for the db.
class Session
{
public:
   // Register a class to the db.
   template<typename C, typename R>
   void mapClass(const std::string& name, const R& defaultValue, bool caseSensitive = true) {
      mapClassImp(name, typeOf<C>(), caseSensitive, json::toJson(defaultValue));
   }

   // Get the class' name.
   template<typename C>
   const std::string& getClassName() const {
      auto y = typeOf<C>();
      assert(_className.count(y) > 0); // Make sure the class was mapped.
      return _className.at(y);
   }

   template<typename C>
   void save(C& ele) {
      auto y = getClassName<C>();
      WriteAction action(std::make_shared<json::JsonObject>(), _impl.getIdGen(y));
      ele.persist(action);
      _impl.save(y, std::dynamic_pointer_cast<json::JsonObject>(action.doc)->value.at(ID), action.doc);
   }

   template<typename C>
   void save(C* ele) {
      auto y = getClassName<C>();
      WriteAction action(std::make_shared<json::JsonObject>(), _impl.getIdGen(y));
      ele->persist(action);
      _impl.save(y, std::dynamic_pointer_cast<json::JsonObject>(action.doc)->value.at(ID), action.doc);
   }

   template<typename C>
   ReadStatement<C> find() {
      return ReadStatement<C> (getClassName<C>(), _impl);
   }

   template<typename C>
   void remove(json::Json_t id) {
      _impl.remove(getClassName<C>(), id);
   }
   template<typename C>
   void remove(C& ele) {
      DeleteAction action;
      ele.persist(action);
      _impl.remove(getClassName<C>(), action.doc->getChild(action.doc, ID_PATH).front());
   }
   template<typename C>
   void remove(C* ele) {
      DeleteAction action;
      ele->persist(action);
      _impl.remove(getClassName<C>(), action.doc->getChild(action.doc, ID_PATH).front());
   }
   template<typename C>
   void remove(Clause& where) {
      _impl.remove(getClassName<C>(), where);
   }

   template<typename C>
   // FIXME: add unique
   void addIndex(const std::string& path, bool caseSensitive=true) {
      _impl.addIndex(getClassName<C>(), path, caseSensitive);
   }

   template<typename C>
   void addId(const std::string& path, bool caseSensitive=true) {
      _impl.addIndex(getClassName<C>(), path, caseSensitive);
   }

   inline jsondb::JsonDb& getDb() {return _impl;}

   inline void scanIndex() {return _impl.scanIndex(); }

protected:
   JsonDb _impl;
   elladan::VMap<const std::type_info*, std::string> _className;

   void mapClassImp(const std::string& clas, const std::type_info* type, bool caseSensitive, json::Json_t defaultValue);

   template <typename C>
   const std::type_info* typeOf() const { return &typeid(C); }
};

} } // namespace elladan::jsondb


/*
 * Controller.h

 *
 *  Created on: Dec 21, 2016
 *      Author: daniel
 */

#pragma once

#include <elladan/json/json.h>
#include <stddef.h>
#include <map>
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
    template<typename C>
    void mapClass(const std::string& name, bool caseSensitive = true) {
        mapClassImp(name, typeid(C).hash_code(), caseSensitive);
    }

    // Get the class' name.
    template<typename C>
    const std::string& getClassName() const {
        return _className.at(typeid(C).hash_code());
    }

    template<typename C>
    void save(C& ele) {
        WriteAction action(std::make_shared<json::JsonObject>());
        ele.persist(action);
        _impl.save(getClassName<C>(), std::dynamic_pointer_cast<json::JsonObject>(action.doc)->value.at(ID), action.doc);
    }

    template<typename C>
    ReadStatement<C> find() {
        return ReadStatement<C> (getClassName<C>(), _impl);
    }

    template<typename C>
    void remove(C& ele) {
        DeleteAction action;
        ele.persist(action);
        _impl.remove(getClassName<C>(), action.doc->getChild(action.doc, ID).front());
    }
    template<typename C>
    void remove(std::shared_ptr<Clause> where) {
        _impl.remove(getClassName<C>(), where);
    }

    template<typename C>
    void addIndex(const std::string& path, bool caseSensitive=true) {
        _impl.addIndex(getClassName<C>(), path, caseSensitive);
    }

    inline jsondb::JsonDb& getDb() {return _impl;}

protected:
    jsondb::JsonDb _impl;
    std::map<size_t, std::string> _className;

    void mapClassImp(const std::string& clas, size_t hash, bool caseSensitive);
};

} } // namespace elladan::jsondb


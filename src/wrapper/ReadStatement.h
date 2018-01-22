/*
 * Statement.h
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#pragma once

#include <elladan/json/json.h>
#include <memory>
#include <string>
#include <vector>

#include "../JsonDb.h"
#include "../Sort.h"
#include "Action.h"

namespace elladan {
namespace jsondb {

/// Manage read only query.
template<typename C>
class ReadStatement {
private:
    inline C deserial(json::JsonObject_t doc) {
        ReadAction action(doc);
        C ele;
        ele.persist(action);
        return ele;
    }

public:
    ReadStatement(const std::string& classPath, jsondb::JsonDb& ctl) :
        _ctl(ctl), _classPath(classPath), _id(std::make_shared<json::Json>()) {
    }

    // Do the actual read and parsing.
    std::vector<C> findAll() {
       std::vector<json::Json_t> toDeserializeList;
       if (_id && _id->getType() != json::JSON_NONE)
           // Load only the specific id.
           toDeserializeList.emplace_back(_ctl.load(_classPath, _id));
       else
           // Load all item.
           toDeserializeList = _ctl.loadConditionnal(_classPath, _clause, _sort);

       std::vector<C> retVal;
       for (auto ite : toDeserializeList)
           retVal.emplace_back(deserial(std::dynamic_pointer_cast<json::JsonObject>(ite)));
       return retVal;
    }

    C findOne() {
        if (_id && _id->getType() != json::JSON_NONE){
            json::Json_t doc = _ctl.load(_classPath, _id);

            // No file loaded, return.
            if (!doc.get() || doc->getType() == json::JSON_NONE)
                return C();

            if (doc->getType() != json::JSON_OBJECT)
                throw Exception("Only supporting object data file");

            // Translate json to object.
            return deserial(std::dynamic_pointer_cast<json::JsonObject>(doc));
        }
        else {
            // Load all json.
            for (auto ite : _ctl.loadConditionnal(_classPath, _clause, _sort))
                return deserial(std::dynamic_pointer_cast<json::JsonObject>(ite));
        }
        return C ();
    }

    size_t count() {
        size_t retVal;

        if (_id->getType() != json::JSON_NONE)
            retVal = (_ctl.count(_classPath, _id));
        else
            // Load all json.
            retVal = _ctl.countConditionnal(_classPath, _clause);

        return retVal;
    }


    // Set the id to search for.
    template<typename T>
    ReadStatement& id(T id) {
        _id = json::toJson(id);
        return *this;
    }

    ReadStatement<C>& where(std::shared_ptr<jsondb::Clause> clause) {
        _clause = clause;
        return *this;
    }

protected:
    json::Json_t _id;
    std::string _classPath;
    jsondb::JsonDb& _ctl;
    std::shared_ptr<jsondb::Clause> _clause;
    jsondb::Sort _sort;

};

} } // namespace elladan::jsondb


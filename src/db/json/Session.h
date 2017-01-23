/*
 * Controller.h

 *
 *  Created on: Dec 21, 2016
 *      Author: daniel
 */

#ifndef SRC_DB_JSON_MANAGER_SESSION_H_
#define SRC_DB_JSON_MANAGER_SESSION_H_

#include <cassert>
#include <map>
#include <string>
#include <typeinfo>

#include "../FileManipulator.h"
#include "Action.h"
#include "Json.h"
#include "ReadStatement.h"
#include "Session_impl.h"

namespace db {
namespace json {


/// This is the main interface for the db.
class Session
{
public:
    Session(){}
    ~Session(){}

    // Set the DB base folder.
    void setBasePath(const std::string& basePath);
    const std::string& basePath() const;

    // Register a class to the db.
    template<typename C>
    void mapClass(const std::string& name) {
        assert(!name.empty());
        std::string cn = removeDoubleSlash(name + "/");
        _session._className[typeid(C).hash_code()] = cn;
        createFolder(_session._basePath + cn);
    }

    // Get the class' name.
    template<typename C>
    const std::string& getClassName() const {
        return _session._className.at(typeid(C).hash_code());
    }

    template<typename C>
    void save(C& ele) {
        db::json::WriteAction action;
        ele.persist(action);
        doSave(action.doc.asObject(), getClassName<C>());
    }

    template<typename C>
    ReadStatement<C> find() {
        return ReadStatement<C> (_session._basePath + getClassName<C>(), _session);
    }

    template<typename C>
    void remove(C& ele) {
        db::json::WriteAction action;
        ele.persist(action);
        // FIXME: too heavy, should be able to read only the id. Create a DELETE action with no-ops bind?
        // FIXME: does not manage related table. With Json, is it still required?
        doRemove(_session._basePath + getClassName<C>() + action.doc.name());
    }

private:
    SessionImp _session;

    void doSave(JsonObject& doc, const std::string& className);
    void doRemove(std::string filename);
};

} /* namespace json */
} /* namespace db */

#endif /* SRC_DB_JSON_MANAGER_SESSION_H_ */

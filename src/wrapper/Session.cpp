/*
 * Controller.cpp
 *
 *  Created on: Dec 21, 2016
 *      Author: daniel
 */

#include "Session.h"

#include <cassert>
#include <elladan/FileManipulator.h>

namespace elladan {
namespace jsondb {

// Register a class to the db.
void Session::mapClassImp(const std::string& name, const std::type_info* type, bool caseSensitive, json::Json_t defaultValue) {
    assert(!name.empty());

    if (!_className.insert(type, name))
       throw elladan::Exception("Class already mapped");

    _impl.setId(name, defaultValue, caseSensitive);
    DEBUG("Mapped class %s", name.c_str());
}

} } // namespace elladan::jsondb


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
void Session::mapClassImp(const std::string& name, size_t hashCode, bool caseSensitive) {
    assert(!name.empty());
    _className[hashCode] = name;
    _impl.addIndex(name, ID, true, caseSensitive);
    DEBUG("Mapped class %s", name.c_str());
}

} } // namespace elladan::jsondb


/*
 * Index.cpp
 *
 *  Created on: Feb 16, 2017
 *      Author: daniel
 */

#include "Index.h"

#include <memory>

namespace elladan { namespace jsondb {

IndexElement::IndexElement() : _uid(nullptr), _value(nullptr) {}
IndexElement::IndexElement(json::Json_t id, json::Json_t value) : _uid(id), _value(value) {}

Index::Index(const std::string& clas, const std::string& searchPath, bool unique, bool caseSensitive)
    : _clas(clas), _searchPath(searchPath), _single(unique), _caseSensitive(caseSensitive)
{ }

bool Index::add(const IndexElement& ele) {
    json::Json_t id = getValue(ele);
    auto ite = _elements.find(id);
    if (_single && ite != _elements.end())
        return false;
    else {
        // Don't add twice the same key-uid pair.
        while (ite != _elements.end()) {
            if (ite->first != id) break;
            if (ite->second._uid == ele._uid) return true;
        }
    }

    if (id->getType() == json::JSON_NONE || id->getType() == json::JSON_NULL)
        throw Exception("invalid indexed ");

    _elements.insert(std::pair<json::Json_t, IndexElement>(id, ele));
    return true;
}

void Index::clear() {
    _elements.clear();
}

void Index::remove(const IndexElement& ele) {
    json::Json_t id = getValue(ele);

    auto ite = _elements.find(id);
    while (ite != _elements.end()) {
        if (ite->first != id) break;
        if (ite->second._uid == ele._uid) ite = _elements.erase(ite);
    }
}

void Index::removeAll(json::Json_t uid) {
    for (auto ite = _elements.begin(); ite != _elements.end();) {
        if (ite->second._uid == uid)
            ite = _elements.erase(ite);
        else ite++;
    }
}

json::Json_t Index::getValue(const IndexElement& ele) {
    json::Json_t id = ele._value;

    // if case insensitive, convert to lower case.
    if (!_caseSensitive && id->getType() == json::JSON_STRING)
        id = json::toJson(toLower(toString(id)));

    return id;
}


} } // namespace elladan::jsondb

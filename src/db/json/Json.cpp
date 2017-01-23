/*
 * Json.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: daniel
 */

#include "Json.h"

#include <stdexcept>
#include <utility>
#include <cassert>

#include "../FileManipulator.h"

namespace db {
namespace json {

JsonElement::JsonElement() :
        _element(json_null()), _parent(json_null()) {
}
JsonElement::~JsonElement() {
    json_decref(_element);
    json_decref(_parent);
}

JsonElement::JsonElement(const std::string& key, JsonObject& parent) :
        JsonElement(key, json_null(), parent) {
}
JsonElement::JsonElement(const std::string& key, JsonList& parent) :
        JsonElement(key, json_null(), parent) {
}

JsonValue& JsonElement::asValue() {
    assert(!isObject() && !isList());
    return static_cast<JsonValue&>(*this);
}
JsonList& JsonElement::asList() {
    assert(isList());
    return static_cast<JsonList&>(*this);
}
JsonObject& JsonElement::asObject() {
    assert(isObject());
    return static_cast<JsonObject&>(*this);
}
JsonElement& JsonElement::asElement() {
    return static_cast<JsonElement&>(*this);
}
const JsonValue& JsonElement::asValue() const {
    assert(!isObject() && !isList());
    return static_cast<const JsonValue&>(*this);
}
const JsonList& JsonElement::asList() const {
    assert(isList());
    return static_cast<const JsonList&>(*this);
}
const JsonObject& JsonElement::asObject() const {
    assert(isObject());
    return static_cast<const JsonObject&>(*this);
}
const JsonElement& JsonElement::asElement() const {
    return static_cast<const JsonElement&>(*this);
}

JsonElement::JsonElement(const std::string& key, json_t* ele, JsonObject& parent) :
        _element(ele), _name(key), _parent(parent._element) {
    assert(parent.isObject());
    json_incref(_parent);
    json_object_set(_parent, key.c_str(), _element);
}

JsonElement::JsonElement(const std::string& key, json_t* ele, JsonList& parent) :
        _element(ele), _name(key), _parent(parent._element) {
    assert(parent.isList());
    json_incref(_parent);
    json_array_append(_parent, _element);
}

JsonValue::JsonValue() :
        JsonElement() {
}
JsonValue::~JsonValue() {
}


JsonValue::JsonValue(std::string key, bool val, JsonObject& parent)                : JsonElement(key, json_boolean(val), parent) {}
JsonValue::JsonValue(std::string key, const std::string& str, JsonObject& parent)  : JsonElement(key, json_string(str.c_str()), parent) {}
JsonValue::JsonValue(std::string key, long val, JsonObject& parent)                : JsonElement(key, json_integer(val), parent) {}
JsonValue::JsonValue(std::string key, double val, JsonObject& parent)              : JsonElement(key, json_real(val), parent) {}
JsonValue::JsonValue(std::string key, JsonObject& parent)                          : JsonElement(key, json_null(), parent) {}

JsonValue::JsonValue(std::string key, bool val, JsonList& parent)                  : JsonElement(key, json_boolean(val), parent) {}
JsonValue::JsonValue(std::string key, const std::string& str, JsonList& parent)    : JsonElement(key, json_string(str.c_str()), parent) {}
JsonValue::JsonValue(std::string key, long val, JsonList& parent)                  : JsonElement(key, json_integer(val), parent) {}
JsonValue::JsonValue(std::string key, double val, JsonList& parent)                : JsonElement(key, json_real(val), parent) {}
JsonValue::JsonValue(std::string key, JsonList& parent)                            : JsonElement(key, json_null(), parent) {}



JsonValue JsonValue::copy() {
    JsonValue ret;
    ret._element = json_copy(_element);
    ret._name = _name;
    ret._parent = json_null();
    return ret;
}

JsonObject::JsonObject() :
        JsonElement() {
    setValue(json_object());
}
JsonObject::JsonObject(const std::string& name, JsonObject& parent) :
        JsonElement(name, json_object(), parent) {
}
JsonObject::JsonObject(const std::string& name, JsonList& parent) :
        JsonElement(name, json_object(), parent) {
}

// Object Accessor.
JsonElement JsonObject::get(const std::string& name) {
    JsonElement child;
    child._parent = _element;
    child._element = json_object_get(_element, name.c_str());
    child._name = name;
    return child;
}

JsonObject::Iterator::Iterator(JsonObject& parent) :
        _parent(parent) {
    doc._parent = _parent._element;
    doc._name = json_object_iter_key(json_object_iter(_parent._element));
    if (!doc._name.empty()) doc._element = json_object_iter_value(json_object_key_to_iter(doc._name.c_str()));
}
JsonObject::Iterator& JsonObject::Iterator::operator++() {
    doc._name = json_object_iter_key(
            json_object_iter_next(_parent._element, json_object_key_to_iter(doc._name.c_str())));
    if (!doc._name.empty()) doc._element = json_object_iter_value(json_object_key_to_iter(doc._name.c_str()));
    return *this;
}
JsonObject::Iterator JsonObject::Iterator::operator++(int) {
    Iterator ite(*this);
    ite.doc._name = json_object_iter_key(
            json_object_iter_next(_parent._element, json_object_key_to_iter(doc._name.c_str())));
    if (!ite.doc._name.empty()) ite.doc._element = json_object_iter_value(json_object_key_to_iter(doc._name.c_str()));
    return ite;
}

JsonObject JsonObject::readFile(FILE* file) {
    json_error_t error;
    JsonObject obj;
    obj._element = json_loadf(file, 0, &error);
    if (error.text) {
        std::string msg = std::string("Error parsing json file ") + error.source + " : " + error.text + " at "
                + std::to_string(error.position) + " (line " + std::to_string(error.line) + " column "
                + std::to_string(error.column) + ")";
        throw(new std::runtime_error(msg));
    }
    return obj;
}
bool JsonObject::writeFile(FILE* file) const {
    json_error_t error;
    return json_dumpf(_element, file, JSON_INDENT(1) | JSON_SORT_KEYS) == 0;
}

JsonObject JsonObject::copy() {
    JsonObject ret;
    ret._element = json_deep_copy(_element);
    ret._name = _name;
    ret._parent = json_null();
    return ret;
}

std::vector<JsonElement> JsonObject::findValues(const std::string& path) const {
    std::vector<JsonElement> result;

    auto parts = tokenize(path, "/");

    std::vector<std::pair<JsonElement, int>> toCheck;
    toCheck.push_back(std::pair<JsonElement, int>(*this, 0));

    // For all branch.
    while (!toCheck.empty()) {
        std::pair<JsonElement, int>& pair = toCheck.back();
        JsonElement node = pair.first;
        int depth = pair.second;
        std::string searchName = depth < parts.size() ? parts[depth] : "";
        toCheck.pop_back();

        // Test if this element is in the requested path.
        // FIXME: add regex support
        if (searchName != "*" && searchName != node.name()) continue;

        // Test if we found the element we are looking for.
        if (depth == parts.size() - 1)
            result.push_back(node);

        // Note the one? Add childs, if any.
        else if (node.isObject()) {
            for (auto ite : node.asObject())
                toCheck.push_back(std::pair<JsonElement, int>(ite, depth + 1));
        } else if (node.isList()) {
            for (auto ite : node.asList())
                toCheck.push_back(std::pair<JsonElement, int>(ite, depth + 1));
        }
    }
    return result;
}

JsonList::JsonList() :
        JsonElement() {
    setValue(json_array());
}
JsonList::JsonList(const std::string& name, JsonObject& parent) :
        JsonElement(name, json_array(), parent) {
}
JsonList::JsonList(const std::string& name, JsonList& parent) :
        JsonElement(name, json_array(), parent) {
}

// Object Accessor.
inline JsonValue JsonList::get(int idx) {
    JsonValue child;
    child._element = json_array_get(_element, idx);
    return child;
}

JsonList JsonList::copy() {
    JsonList ret;
    ret._element = json_deep_copy(_element);
    ret._name = _name;
    ret._parent = json_null();
    return ret;
}

JsonList::Iterator::Iterator(JsonList& parent) :
        _parent(parent) {
    doc._name = json_object_iter_key(json_object_iter(_parent._element));
    if (!doc._name.empty()) doc._element = json_object_iter_value(json_object_key_to_iter(doc._name.c_str()));
}
JsonList::Iterator& JsonList::Iterator::operator++() {
    doc._name = json_object_iter_key(
            json_object_iter_next(_parent._element, json_object_key_to_iter(doc._name.c_str())));
    if (!doc._name.empty()) doc._element = json_object_iter_value(json_object_key_to_iter(doc._name.c_str()));
    return *this;
}
JsonList::Iterator JsonList::Iterator::operator++(int) {
    Iterator ite(*this);
    ite.doc._name = json_object_iter_key(
            json_object_iter_next(_parent._element, json_object_key_to_iter(doc._name.c_str())));
    if (!ite.doc._name.empty()) ite.doc._element = json_object_iter_value(json_object_key_to_iter(doc._name.c_str()));
    return ite;
}

} /* namespace json */
} /* namespace db */

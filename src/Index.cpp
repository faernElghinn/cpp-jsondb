/*
 * Index.cpp
 *
 *  Created on: Feb 16, 2017
 *      Author: daniel
 */

#include "Index.h"
#include "JsonDb.h"

#include <memory>
#include <cassert>

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

    if (_searchPath == ID){
        std::unique_lock<std::mutex> guard(_mutex);
        if (!_lastVal.get() || ele._value->cmp(_lastVal.get()) > 0)
            _lastVal = ele._value;
    }

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
        id->toString()->value = toLower(id->toString()->value);

    return id;
}


IdIndex::IdIndex(const std::string& clas, bool caseSensitive, json::Json_t defaultValue)
    : Index(clas, ID, true, caseSensitive), _defaultVal(defaultValue), _lastVal(defaultValue)
{ }

static void increaseVal(std::string& val, bool caseSensitive){
    // Increase last letter. If the letter goes beyond 'z', go back to 'a' and increase next letter.
    int offset = 0;
    bool needToUpgradeNext;
    do{
        offset++;

        if (offset <= val.size()) {
            auto& ch = val[val.size()-offset];
            ++ch;

            needToUpgradeNext = false;
            if (caseSensitive && ch < 'A') {
                ch = 'A';
            }
            else if (caseSensitive && ch <= 'Z') { }
            else if (ch < 'a') {
                ch = 'a';
            }
            else if (ch <= 'z' ) { }
            else {
                ch = caseSensitive ? 'A' : 'a';
                needToUpgradeNext = true;
            }
        }

        // No more letter to increase, append one more.
        else{
            needToUpgradeNext = false;
            val.push_back(caseSensitive ? 'A' : 'a');
        }

    } while(needToUpgradeNext);
}

static void increaseVal(UUID& val) {
    int offset = 0;
    bool needToUpgradeNext;
    do{
        offset++;
        val.getRaw()[offset]++;
    } while(val.getRaw()[offset] == 0);
}

bool IdIndex::add(const IndexElement& ele){
    if (ele._value.get() && _lastVal.get() && ele._value->cmp(_lastVal.get()) > 0)
        _lastVal = ele._value;

    return Index::add(ele);
}

json::Json_t IdIndex::getNextLogical() {
    assert(_searchPath == ID);

    std::unique_lock<std::mutex> guard(_mutex);

    json::Json_t last = _lastVal;
    json::Json* li = last.get();
    switch (li->getType()) {
        case json::JSON_INTEGER:
        {
            auto id = std::make_shared<json::JsonInt>(li->toInt()->value + 1);
            while(_elements.count(id) > 0 || id->cmp(_defaultVal.get()) == 0)
                id->value++;
            _lastVal = id;
            return id;
        }

        case json::JSON_STRING:
        {
            json::JsonString_t id = std::make_shared<json::JsonString>(li->toString()->value);
            increaseVal(id->value, _caseSensitive);
            while(_elements.count(id) > 0 || id->cmp(_defaultVal.get()) == 0)
                increaseVal(id->value, _caseSensitive);
            _lastVal = id;
            return id;
        }

        case json::JSON_UUID:
        {
            json::JsonUUID_t id = std::make_shared<json::JsonUUID>(UUID::generateUUID());
            while(_elements.count(id) > 0 || id->cmp(_defaultVal.get()) == 0)
                increaseVal(id->value);
            _lastVal = id;
            return id;
        }

        default:
            throw Exception("Type not supported as an idx. How did you get it in here?");
    }
}

} } // namespace elladan::jsondb

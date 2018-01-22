/*
 * Index.h
 *
 *  Created on: Feb 16, 2017
 *      Author: daniel
 */

#pragma once

#include <elladan/json/json.h>
#include <map>
#include <string>

namespace elladan { namespace jsondb {

// What should be indexed?

// What is indexed?
struct IndexElement
{
    json::Json_t _uid; /// ID of the parent element.
    json::Json_t _value; /// Actual value.

    IndexElement();
    IndexElement(json::Json_t id, json::Json_t value);

};

// How does it hold together?
class Index
{
public:
    Index() : _caseSensitive(true), _single(false) {};
    Index(const std::string& clas, const std::string& _search_path, bool unique = false, bool caseSensitive = false);
    bool add(const IndexElement& ele);
    void clear();
    void remove(const IndexElement& ele);
    void removeAll(json::Json_t _uid);
    const inline std::multimap<json::Json_t, IndexElement, json::cmpJson>& getElements() const {
        return _elements;
    }
    static bool JsonValueIsLower();
protected:
    inline json::Json_t getValue(const IndexElement& ele);
    std::multimap<json::Json_t, IndexElement, json::cmpJson> _elements;
    std::string _clas;          /// Source class
    std::string _searchPath;   /// Element path. Support wildcards.
    bool _single;
    bool _caseSensitive;
    // FIXME: how is it populated?
};

} } // namespace elladan::jsondb


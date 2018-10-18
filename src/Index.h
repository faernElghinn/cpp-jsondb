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
#include <mutex>

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
    virtual ~Index(){}

    virtual bool add(const IndexElement& ele);
    void clear();
    void remove(const IndexElement& ele);
    void removeAll(json::Json_t _uid);
    inline const std::multimap<json::Json_t, IndexElement, json::cmpJson>& getElements() const { return _elements; }
protected:
    inline json::Json_t getValue(const IndexElement& ele);

    json::Json_t _defaultVal;
    json::Json_t _lastVal;
    std::mutex _mutex;

    std::multimap<json::Json_t, IndexElement, json::cmpJson> _elements;
    std::string _clas;         /// Source class
    std::string _searchPath;   /// Element path. Support wildcards.
    bool _single;
    bool _caseSensitive;
};

// How does it hold together?
class IdIndex : public Index
{
public:
    IdIndex(const std::string& clas, bool caseSensitive, json::Json_t defaultValue);
    json::Json_t getNextLogical();
    inline const json::Json_t defaultValue() { return _defaultVal; }
    bool add(const IndexElement& ele);
protected:
    inline json::Json_t getValue(const IndexElement& ele);

    json::Json_t _defaultVal;
    json::Json_t _lastVal;
    std::mutex _mutex;
};

} } // namespace elladan::jsondb


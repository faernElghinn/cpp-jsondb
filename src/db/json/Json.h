/*
 * Json.h
 *
 *  Created on: Dec 19, 2016
 *      Author: daniel
 */

#ifndef SRC_JSON_H_
#define SRC_JSON_H_

#include <jansson.h>
#include <cstdio>
#include <iterator>
#include <string>
#include <vector>

namespace db {
namespace json {

class JsonElement;
class JsonValue;
class JsonObject;
class JsonList;

class JsonElement
{
public:
    enum Type
    {
        Object = JSON_OBJECT,
        List = JSON_ARRAY,
        String = JSON_STRING,
        Integer = JSON_INTEGER,
        Float = JSON_REAL,
        Boolean_True = JSON_TRUE,
        Boolean_False = JSON_FALSE,
        Null = JSON_NULL
    };

    JsonElement();
    virtual ~JsonElement();
    JsonElement(const std::string& key, JsonObject& parent);
    JsonElement(const std::string& key, JsonList& parent);

    // Type testing.
    inline Type type() const {
        return (Type) json_typeof(_element);
    }
    inline bool isObject() const {
        return json_is_object(_element);
    }
    inline bool isList() const {
        return json_is_array(_element);
    }
    inline bool isString() const {
        return json_is_string(_element);
    }
    inline bool isInt() const {
        return json_is_integer(_element);
    }
    inline bool isFloat() const {
        return json_is_real(_element);
    }
    inline bool isBool() const {
        return json_is_boolean(_element);
    }
    inline bool isNull() const {
        return !_element || json_is_null(_element);
    }

    // Allow to convert between type of element.
    JsonValue& asValue();
    JsonList& asList();
    JsonObject& asObject();
    const JsonValue& asValue() const ;
    const JsonList& asList() const ;
    const JsonObject& asObject() const ;
    JsonElement& asElement();
    const JsonElement& asElement() const ;

    inline bool operator==(const JsonElement& other) const {
        return json_equal(_element, other._element);
    }
    inline bool operator!=(const JsonElement& other) const {
        return !json_equal(_element, other._element);
    }

    inline const std::string& name() const {
        return _name;
    }

    inline void setName(const std::string& name) {
        _name = name;
    }

protected:
    // Gre! Not necessary since everyone inherit from this class!
    friend class JsonValue;
    friend class JsonObject;
    friend class JsonList;
    JsonElement(const std::string& key, json_t* ele, JsonObject& parent);
    JsonElement(const std::string& key, json_t* ele, JsonList& parent);

    inline void setValue(json_t* type) {
        if (_element) json_decref(_element);
        _element = type;
    }

    json_t* _parent;
    json_t* _element;
    std::string _name;
};

class JsonValue: public JsonElement
{
public:
    JsonValue();
    ~JsonValue();

    JsonValue(std::string key, bool val, JsonObject& parent);
    JsonValue(std::string key, const std::string& str, JsonObject& parent);
    JsonValue(std::string key, long val, JsonObject& parent);
    JsonValue(std::string key, double val, JsonObject& parent);
    JsonValue(std::string key, JsonObject& parent);

    JsonValue(std::string key, bool val, JsonList& parent);
    JsonValue(std::string key, const std::string& str, JsonList& parent);
    JsonValue(std::string key, long val, JsonList& parent);
    JsonValue(std::string key, double val, JsonList& parent);
    JsonValue(std::string key, JsonList& parent);

    // Getting a value.
    inline bool getBool() {
        return json_boolean_value(_element);
    }
    inline std::string getString() {
        return json_string_value(_element);
    }
    inline long long getInt() {
        return json_integer_value(_element);
    }
    inline double getFloat() {
        return json_real_value(_element);
    }

    // Setting a value. Should NOT change type.
    inline void setValue(const std::string& str) {
        json_string_set(_element, str.c_str());
    }
    inline void setValue(long val) {
        json_integer_set(_element, val);
    }
    inline void setValue(double val) {
        json_real_set(_element, val);
    }

    JsonValue copy();
};

class JsonObject: public JsonElement
{
public:
    JsonObject();
    JsonObject(const std::string& name, JsonObject& parent);
    JsonObject(const std::string& name, JsonList& parent);

    inline void clear() {
        json_object_clear(_element);
    }
    inline int size() {
        return json_object_size(_element);
    }

    // Object Accessor.
    JsonElement get(const std::string& name);
    inline JsonElement operator[](const std::string& name) {
        return get(name);
    }
    inline void erase(const std::string& name) {
        json_object_del(_element, name.c_str());
    }

    class Iterator : public std::iterator<std::forward_iterator_tag, JsonElement>
    {
    private:
        JsonObject& _parent;
    public:
        JsonElement doc;

        Iterator(JsonObject& parent);
        Iterator& operator++();
        Iterator operator++(int);

        inline bool operator==(const Iterator& rhs) {
            return _parent == rhs._parent && doc == rhs.doc;
        }
        inline bool operator!=(const Iterator& rhs) {
            return _parent != rhs._parent || doc != rhs.doc;
        }
        inline JsonElement& operator*() {
            return doc;
        }
        inline JsonElement& operator->() {
            return doc;
        }
    };

    Iterator begin(){
        return Iterator(*this);
    }
    Iterator end(){
        auto ite = Iterator(*this);
        ite.doc = JsonElement();
        return ite;
    }

    static JsonObject readFile(FILE* file);
    bool writeFile(FILE* file) const;

    JsonObject copy();

    std::vector<JsonElement> findValues(const std::string& path) const;
};

class JsonList: public JsonElement
{
public:
    JsonList();
    JsonList(const std::string& name, JsonObject& parent);
    JsonList(const std::string& name, JsonList& parent);

    inline void clear();
    inline int size();

    // Object Accessor.
    JsonValue get(int idx);
    inline JsonElement operator[](int idx) {
        return get(idx);
    }
    inline void erase(int idx) {
        json_array_remove(_element, idx);
    }

    JsonList copy();

    class Iterator: public std::iterator<std::forward_iterator_tag, JsonElement>
    {
    private:
        JsonList& _parent;
    public:
        JsonElement doc;

        Iterator(JsonList& parent);
        Iterator& operator++();
        Iterator operator++(int);

        inline bool operator==(const Iterator& rhs) {
            return doc == rhs.doc;
        }
        inline bool operator!=(const Iterator& rhs) {
            return doc != rhs.doc;
        }
        inline JsonElement& operator*() {
            return doc;
        }
        inline JsonElement& operator->() {
            return doc;
        }
    };

    Iterator begin(){
        return Iterator(*this);
    }
    Iterator end(){
        auto ite = Iterator(*this);
        ite.doc = JsonElement();
        return ite;
    }

};

} /* namespace json */
} /* namespace db */

#endif /* SRC_JSON_H_ */

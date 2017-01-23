/*
 * Json.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: daniel
 */

#include "Action.h"
#include "../Convertor.h"

#include <utility>

namespace db {
namespace json {


#define SpecializeRead(Type, Action) \
template <> \
void ReadAction::doAction<Type> (Type& ele, const char* name){ \
    if (!doc.isObject()) throw JsonException(std::string("Invalid read - ") + doc.name() + " is not an object.");\
    auto val = doc.asObject().get(name);\
    if (!val.isNull()) throw JsonException(std::string("Invalid read - ") + name + " found.");\
    if (!val.is##Action()) throw JsonException(std::string("Invalid read - ") + name + " is not an " #Type ".");\
    ele = doc.asValue().get##Action();\
}
SpecializeRead(bool, Bool);
SpecializeRead(int,  Int);
SpecializeRead(float, Float);
SpecializeRead(double, Float);
SpecializeRead(std::string, String);
#undef SpecializeRead


#define SpecializeReadId(Type, Action) \
template <> \
void ReadAction::doActionId<Type> (Type& ele, bool autoGen){ \
    if (!doc.isObject()) throw JsonException(std::string("Invalid read - ") + doc.name() + " is not an object.");\
    auto val = doc.asObject().get("id");\
    if (!val.isNull()) throw JsonException(std::string("Invalid read - id found."));\
    if (!val.is##Action()) throw JsonException(std::string("Invalid read - id is not an " #Type "."));\
    ele = doc.asValue().get##Action();\
}
SpecializeReadId(int,  Int);
SpecializeReadId(std::string, String);
#undef SpecializeReadId


#define SpecializeReadList(Type, Action) \
template <>\
void ReadAction::doActionList<Type>(std::vector<Type>& ele, const char* name){\
    if (!doc.isObject()) throw JsonException(std::string("Invalid read - ") + doc.name() + " is not an object.");\
    auto val = doc.asObject().get(name);\
    if (!val.isNull()) throw JsonException(std::string("Invalid read - ") + name + " not found.");\
    if (!val.isList()) throw JsonException(std::string("Invalid read - ") + name + " is not a list.");\
    ele.clear();\
    for (auto& ite : val.asList()) {\
        if (!ite.is##Action()) throw JsonException(std::string("Invalid read - item ") + ite.asValue().getString() + "of " + name + " is not an " #Type ".");\
        ele.push_back(doc.asValue().get##Action());\
    }\
}
SpecializeReadList(bool, Bool);
SpecializeReadList(int,  Int);
SpecializeReadList(float, Float);
SpecializeReadList(double, Float);
SpecializeReadList(std::string, String);
#undef SpecializeReadList


#define SpecializeReadMap(Type, Action) \
template <>\
void ReadAction::doActionMap<Type>(std::map<std::string, Type>& ele, const char* name){\
    if (!doc.isObject()) throw JsonException(std::string("Invalid read - ") + doc.name() + " is not an object.");\
    auto val = doc.asObject().get(name);\
    if (!val.isNull()) throw JsonException(std::string("Invalid read - ") + name + " not found.");\
    if (!val.isObject()) throw JsonException(std::string("Invalid read - ") + name + " is not a list.");\
    ele.clear();\
    for (auto& ite : val.asObject()) {\
        if (!ite.is##Action()) throw JsonException(std::string("Invalid read - item ") + ite.name() + "of " + name + " is not an " #Type ".");\
        ele[ite.name()] = (doc.asValue().get##Action());\
    }\
}
SpecializeReadMap(bool, Bool);
SpecializeReadMap(int,  Int);
SpecializeReadMap(float, Float);
SpecializeReadMap(double, Float);
SpecializeReadMap(std::string, String);
#undef SpecializeReadMap





#define SpecializeWrite(Type, Cast) \
template <> \
void WriteAction::doAction<Type>(Type& ele, const char* name){ \
    if (!doc.isObject()) throw JsonException(std::string("Invalid write - ") + doc.name() + " is not an object.");\
    JsonObject obj = doc.asObject();\
    JsonValue(name, (Cast)ele, obj);\
}
SpecializeWrite(bool, bool);
SpecializeWrite(int, long);
SpecializeWrite(float, double);
SpecializeWrite(double, double);
SpecializeWrite(std::string, std::string);
#undef SpecializeWrite


// FIXME: manage autoGen
#define SpecializeWriteId(Type, Cast) \
template <> \
void WriteAction::doActionId<Type>(Type& ele, bool autoGen){ \
    if (!doc.isObject()) throw JsonException(std::string("Invalid write - ") + doc.name() + " is not an object.");\
    JsonObject obj = doc.asObject();\
    JsonValue("id", (Cast)ele, obj);\
    doc.setName(toString(ele));\
}
SpecializeWriteId(int, long);
SpecializeWriteId(std::string, std::string);
#undef SpecializeWrite


#define SpecializeWriteList(Type, Cast) \
template <>\
void WriteAction::doActionList<Type>(std::vector<Type>& ele, const char* name) {\
    if (!doc.isObject()) throw JsonException(std::string("Invalid write - ") + doc.name() + " is not an object.");\
    JsonObject obj = doc.asObject();\
    auto list = JsonList(name, obj);\
    for (auto ite : ele)\
        JsonValue("", (Cast)ite, list);\
}
SpecializeWriteList(bool, bool);
SpecializeWriteList(int, long);
SpecializeWriteList(float, double);
SpecializeWriteList(double, double);
SpecializeWriteList(std::string, std::string);
#undef SpecializeWriteList


#define SpecializeWriteMap(Type, Cast) \
template <>\
void WriteAction::doActionMap<Type>(std::map<std::string, Type>& ele, const char* name){\
    if (!doc.isObject()) throw JsonException(std::string("Invalid write - ") + doc.name() + " is not an object.");\
    JsonObject obj = doc.asObject();\
    auto list = JsonObject(name, obj);\
    for (auto& ite : ele){\
        JsonObject sub(ite.first, list);\
        JsonValue(ite.first, (Cast)ite.second, sub);\
    }\
}
SpecializeWriteMap(bool, bool);
SpecializeWriteMap(int, long);
SpecializeWriteMap(float, double);
SpecializeWriteMap(double, double);
SpecializeWriteMap(std::string, std::string);
#undef SpecializeWriteMap


#define SpecializeDeleteId(Type, Cast) \
template <> \
void DeleteAction::doActionId<Type>(Type& ele, bool autoGen){ \
    if (!doc.isObject()) throw JsonException(std::string("Invalid write - ") + doc.name() + " is not an object.");\
    JsonObject obj = doc.asObject();\
    JsonValue("id", (Cast)ele, obj);\
    doc.setName(toString(ele));\
}
SpecializeDeleteId(int, long);
SpecializeDeleteId(std::string, std::string);
#undef SpecializeDeleteId



// Special char cases.
template <>
void ReadAction::doAction<char>(char& ele, const char* name){
    std::string val;
    doAction(val, name);
    ele = val.front();
}
template <>
void ReadAction::doActionList<char>(std::vector<char>& ele, const char* name){
    std::vector<std::string> val;
    doActionList(val, name);
    ele.clear();
    for(auto& ite : val)
        ele.push_back(ite.front());
}
template <>
void ReadAction::doActionMap<char>(std::map<std::string, char>& ele, const char* name){
    std::map<std::string, std::string> val;
    doActionMap(val, name);
    ele.clear();
    for(auto& ite : val)
        ele[ite.first] = ite.second.front();
}
template <>
void WriteAction::doAction<char>(char& ele, const char* name){
    std::string val = &ele;
    WriteAction::doAction(val, name);
}
template <>
void WriteAction::doActionList<char>(std::vector<char>& ele, const char* name){
    std::vector<std::string> val;
    for (auto ite : ele)
        val.push_back(&ite);
    doActionList(val, name);
}
template <>
void WriteAction::doActionMap<char>(std::map<std::string, char>& ele, const char* name){
    std::map<std::string, std::string> val;
    for (auto ite : ele)
        val[ite.first] = ite.second;
    doActionMap(val, name);
}


// Special UUID cases.
template <>
void ReadAction::doAction<UUID>(UUID& ele, const char* name){
    std::string val;
    doAction(val, name);
    ele = UUID::fromString(val);
}
template <>
void ReadAction::doActionId<UUID>(UUID& ele, bool autoGen){
    std::string val;
    doActionId(val, autoGen);
    ele = UUID::fromString(val);
}
template <>
void ReadAction::doActionList<UUID>(std::vector<UUID>& ele, const char* name){
    std::vector<std::string> val;
    doActionList(val, name);
    ele.clear();
    for(auto& ite : val)
        ele.push_back(UUID::fromString(ite));
}
template <>
void ReadAction::doActionMap<UUID>(std::map<std::string, UUID>& ele, const char* name){
    std::map<std::string, std::string> val;
    doActionMap(val, name);
    ele.clear();
    for(auto& ite : val)
        ele[ite.first] = UUID::fromString(ite.second);
}
template <>
void WriteAction::doAction<UUID>(UUID& ele, const char* name){
    auto val = ele.toString();
    doAction(val, name);
}
template <>
void WriteAction::doActionId<UUID>(UUID& ele, bool autoGen){
    auto val = ele.toString();
    doActionId(val, autoGen);
}
template <>
void WriteAction::doActionList<UUID>(std::vector<UUID>& ele, const char* name){
    std::vector<std::string> val;
    for (auto ite : ele)
        val.push_back(ite.toString());
    doActionList(val, name);
}
template <>
void WriteAction::doActionMap<UUID>(std::map<std::string, UUID>& ele, const char* name){
    std::map<std::string, std::string> val;
    for (auto ite : ele)
        val[ite.first] = ite.second.toString();
    doActionMap(val, name);
}

template <>
void DeleteAction::doActionId<UUID>(UUID& ele, bool autoGen){
    auto val = ele.toString();
    doActionId(val, autoGen);
}


} /* namespace json */
} /* namespace db */

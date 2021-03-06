/*
 * Json.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: daniel
 */

#include "Action.h"
#include "../JsonDb.h"
#include "../Index.h"

#include <cassert>

namespace elladan {
namespace jsondb {

using namespace json;
class IdIndex;
inline void ReadAction::validateDoc(const std::string& error) {
    if (!doc || doc->getType() != json::JSON_OBJECT)
        throw Exception("Invalid " + error + " read - " + path + " is not an object");
}

inline json::Json_t ReadAction::getValue(const std::string& name, const std::string& error) {
    auto ite = doc->value.find(name);
    if (ite == doc->value.end())
        throw Exception("Invalid " + error +" read - " + name + " is not part of " + path);
    return ite->second;
}

//json::JsonObject_t WriteAction::validateDoc(const std::string& error) {
//    if (!doc || doc->getType() != json::JSON_OBJECT)
//        throw Exception("Invalid " + error + " write - " + path + " is not an object");
//    return std::dynamic_pointer_cast<json::JsonObject>(doc);
//}


json::JsonObject_t DeleteAction::validateDoc(const std::string& error) {
    if (!doc || doc->getType() != json::JSON_OBJECT)
        throw Exception("Invalid " + error + " delete - " + path + " is not an object");
    return std::dynamic_pointer_cast<json::JsonObject>(doc);
}


#define SpecializeReadId(Type) \
template <> \
void ReadAction::doActionId<Type> (Type& ele, bool autoGen){ \
    doAction(ele, ID);\
}
SpecializeReadId(int);
SpecializeReadId(std::string);
SpecializeReadId(UUID);
#undef SpecializeReadId


#define SpecializeWriteId(Type) \
template <> \
void WriteAction::doActionId<Type>(Type& ele, bool autoGen){ \
    assert(_idx.get() != nullptr);\
    if (autoGen && elladan::json::toJson(ele) == _idx->defaultValue()) {\
        ele = json::fromJson<Type>(_idx->getNextLogical());\
    }\
    doAction(ele, ID);\
}
SpecializeWriteId(int);
SpecializeWriteId(std::string);
SpecializeWriteId(UUID);
#undef SpecializeWrite


#define SpecializeDeleteId(Type) \
template <> \
void DeleteAction::doActionId<Type>(Type& ele, bool autoGen){ \
    validateDoc("id")->value[ID] = json::toJson(ele);\
}
SpecializeDeleteId(int);
SpecializeDeleteId(std::string);
SpecializeDeleteId(UUID);
#undef SpecializeDeleteId





} } // namespace elladan::jsondb

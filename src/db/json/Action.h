/*
 * Action.h
 *
 *  Created on: Dec 19, 2016
 *      Author: daniel
 */

#ifndef SRC_ACTION_H_
#define SRC_ACTION_H_

#include <map>
#include <string>
#include <vector>

#include "../UUID.h"
#include "Json.h"
#include "JsonException.h"

namespace db {
namespace json {

// Read Action
class ReadAction
{
public:
    ReadAction(JsonObject& obj) : doc(obj.asElement()) { }

    template<typename Element>
    void doAction(Element& ele, const char* name) {
        if (!doc.isObject()) throw JsonException(std::string("Invalid read - ") + doc.name() + " is not an object.");
        ReadAction subAction = *this;
        subAction.doc = doc.asObject().get(name);
        ele.persist(subAction);
    }

    template<typename Element>
    void doActionId(Element& ele, bool autoGen) {
        throw JsonException(std::string("Invalid read - ID only support type supported by toString "));
    }

    template<typename Element>
    void doActionList(std::vector<Element>& ele, const char* name) {
        if (!doc.isObject())
            throw JsonException(std::string("Invalid array read - ") + doc.name() + " is not an object.");

        auto list = doc.asObject().get(name);
        if (!list.isList()) throw JsonException(std::string("Invalid array read - ") + name + " is not a list.");

        for (auto& ite : list.asList()) {
            Element element;
            ReadAction subAction = *this;
            subAction.doc = ite;
            element.persist(subAction);
            ele.push_back(element);
        }
    }

    template<typename Element>
    void doActionMap(std::map<std::string, Element>& ele, const char* name) {
        if (!doc.isObject())
            throw JsonException(std::string("Invalid map read - ") + doc.name() + " is not an object.");

        auto obj = doc.asObject().get(name);
        if (!obj.isObject()) throw JsonException(std::string("Invalid array read - ") + name + " is not an object.");

        for (auto& ite : obj.asObject()) {
            ReadAction subAction = *this;
            subAction.doc = ite;
            ele[ite.name()].persist(subAction);
        }
    }
    JsonElement doc;
};

// Direct
template<> void ReadAction::doAction<bool>(bool& ele, const char* name);
template<> void ReadAction::doAction<int>(int& ele, const char* name);
template<> void ReadAction::doAction<double>(double& ele, const char* name);
template<> void ReadAction::doAction<float>(float& ele, const char* name);
template<> void ReadAction::doAction<char>(char& ele, const char* name);
template<> void ReadAction::doAction<std::string>(std::string& ele, const char* name);
template<> void ReadAction::doAction<UUID>(UUID& ele, const char* name);

// Id
template<> void ReadAction::doActionId<int>(int& ele, bool autoGen);
template<> void ReadAction::doActionId<std::string>(std::string& ele, bool autoGen);
template<> void ReadAction::doActionId<UUID>(UUID& ele, bool autoGen);

// list
template<> void ReadAction::doActionList<bool>(std::vector<bool>& ele, const char* name);
template<> void ReadAction::doActionList<int>(std::vector<int>& ele, const char* name);
template<> void ReadAction::doActionList<double>(std::vector<double>& ele, const char* name);
template<> void ReadAction::doActionList<float>(std::vector<float>& ele, const char* name);
template<> void ReadAction::doActionList<char>(std::vector<char>& ele, const char* name);
template<> void ReadAction::doActionList<std::string>(std::vector<std::string>& ele, const char* name);
template<> void ReadAction::doActionList<UUID>(std::vector<UUID>& ele, const char* name);

// map
template<> void ReadAction::doActionMap<bool>(std::map<std::string, bool>& ele, const char* name);
template<> void ReadAction::doActionMap<int>(std::map<std::string, int>& ele, const char* name);
template<> void ReadAction::doActionMap<double>(std::map<std::string, double>& ele, const char* name);
template<> void ReadAction::doActionMap<float>(std::map<std::string, float>& ele, const char* name);
template<> void ReadAction::doActionMap<char>(std::map<std::string, char>& ele, const char* name);
template<> void ReadAction::doActionMap<std::string>(std::map<std::string, std::string>& ele, const char* name);
template<> void ReadAction::doActionMap<UUID>(std::map<std::string, UUID>& ele, const char* name);

// Write Action
class WriteAction
{
public:
    WriteAction() : doc(JsonObject().asElement()) {}

    template<typename Element>
    void doAction(Element& ele, const char* name) {
        if (!doc.isObject()) throw JsonException(std::string("Invalid write - ") + doc.name() + " is not an object.");
        WriteAction subAction = *this;
        subAction.doc = JsonObject(name, doc.asObject()).asElement();
        ele.persist(subAction);
    }

    template<typename Element>
    void doActionId(Element& ele, bool autoGen) {
        throw JsonException(std::string("Invalid write - ID only support type supported by toString "));
    }

    template<typename Element>
    void doActionList(std::vector<Element>& ele, const char* name) {
        if (!doc.isObject())
            throw JsonException(std::string("Invalid list write - ") + doc.name() + " is not an object.");
        auto object = JsonList(name, doc);

        for (auto& ite : ele) {
            WriteAction subAction = *this;
            subAction.doc = JsonObject(name, doc.asObject()).asElement();
            ite.persist(subAction);
        }
    }

    template<typename Element>
    void doActionMap(std::map<std::string, Element>& ele, const char* name) {
        if (!doc.isObject())
            throw JsonException(std::string("Invalid map write - ") + doc.name() + " is not an object.");
        auto object = JsonObject(name, doc);

        for (auto& ite : ele) {
            WriteAction subAction = *this;
            subAction.doc = JsonObject(ite.first, object);
            ite.second.persist(subAction);
        }
    }

    JsonElement doc;
};

// Direct
template<> void WriteAction::doAction<bool>(bool& ele, const char* name);
template<> void WriteAction::doAction<int>(int& ele, const char* name);
template<> void WriteAction::doAction<double>(double& ele, const char* name);
template<> void WriteAction::doAction<float>(float& ele, const char* name);
template<> void WriteAction::doAction<char>(char& ele, const char* name);
template<> void WriteAction::doAction<std::string>(std::string& ele, const char* name);
template<> void WriteAction::doAction<UUID>(UUID& ele, const char* name);

// list
template<> void WriteAction::doActionList<bool>(std::vector<bool>& ele, const char* name);
template<> void WriteAction::doActionList<int>(std::vector<int>& ele, const char* name);
template<> void WriteAction::doActionList<double>(std::vector<double>& ele, const char* name);
template<> void WriteAction::doActionList<float>(std::vector<float>& ele, const char* name);
template<> void WriteAction::doActionList<char>(std::vector<char>& ele, const char* name);
template<> void WriteAction::doActionList<std::string>(std::vector<std::string>& ele, const char* name);
template<> void WriteAction::doActionList<UUID>(std::vector<UUID>& ele, const char* name);

// map
template<> void WriteAction::doActionMap<bool>(std::map<std::string, bool>& ele, const char* name);
template<> void WriteAction::doActionMap<int>(std::map<std::string, int>& ele, const char* name);
template<> void WriteAction::doActionMap<double>(std::map<std::string, double>& ele, const char* name);
template<> void WriteAction::doActionMap<float>(std::map<std::string, float>& ele, const char* name);
template<> void WriteAction::doActionMap<char>(std::map<std::string, char>& ele, const char* name);
template<> void WriteAction::doActionMap<std::string>(std::map<std::string, std::string>& ele, const char* name);
template<> void WriteAction::doActionMap<UUID>(std::map<std::string, UUID>& ele, const char* name);

// Id
template<> void WriteAction::doActionId<int>(int& ele, bool autoGen);
template<> void WriteAction::doActionId<std::string>(std::string& ele, bool autoGen);
template<> void WriteAction::doActionId<UUID>(UUID& ele, bool autoGen);


// Write Action
class DeleteAction
{
public:
    DeleteAction() : doc(JsonObject().asElement()) {}

    template<typename Element>
    void doAction(Element& ele, const char* name) {    }

    template<typename Element>
    void doActionId(Element& ele, bool autoGen) {
        throw JsonException(std::string("Invalid delete - ID only support type supported by toString "));
    }

    template<typename Element>
    void doActionList(std::vector<Element>& ele, const char* name) {   }

    template<typename Element>
    void doActionMap(std::map<std::string, Element>& ele, const char* name) {    }

    JsonElement doc;
};

// Id
template<> void DeleteAction::doActionId<int>(int& ele, bool autoGen);
template<> void DeleteAction::doActionId<std::string>(std::string& ele, bool autoGen);
template<> void DeleteAction::doActionId<UUID>(UUID& ele, bool autoGen);



template<typename Action, typename Element>
void field(Action a, Element& ele, const char* name) {
    a.doAction(ele, name);
}
template<typename Action, typename Element>
void map(Action a, Element& ele, const char* name) {
    a.doActionMap(ele, name);
}
template<typename Action, typename Element>
void list(Action a, Element& ele, const char* name) {
    a.doActionList(ele, name);
}
template<typename Action, typename Element>
void id(Action a, Element& ele, bool autoGen = true) {
    a.doActionId(ele, autoGen);
}

} /* namespace json */
} /* namespace db */

#endif /* ACTION_H_ */

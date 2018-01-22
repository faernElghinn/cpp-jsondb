/*
 * JsonDbTest.cpp
 *
 *  Created on: Jan 10, 2017
 *      Author: daniel
 */

#include <elladan/json/json.h>
#include <elladan/UUID.h>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../src/Clause.h"
#include "../src/JsonDb.h"
#include "../src/Sort.h"
#include "Test.h"

const std::string tableName = "test";

static UUID id;
static JsonObject_t root;
static UUID id2;
static JsonObject_t root2;

static std::string test_json_db_setup(){
    system("rm -rf data");
    id = UUID::generateUUID();

    root = std::make_shared<JsonObject>();
    root->value["id"] = std::make_shared<JsonString>(id.toString());
    root->value["test1"] = std::make_shared<JsonBool>(true);
    root->value["test2"] = std::make_shared<JsonBool>(false);

    id2 = UUID::generateUUID();
    root2 = std::make_shared<JsonObject>();
    root2->value["id"] = std::make_shared<JsonString>(id2.toString());
    root2->value["test1"] = std::make_shared<JsonBool>(false);
    root2->value["test2"] = std::make_shared<JsonBool>(true);

    return "";
}

static std::string test_json_db_save(){
    std::string retVal;

    JsonDb db;
    db.save(tableName, std::make_shared<JsonString>(id.toString()), root);
    db.save(tableName, std::make_shared<JsonString>(id2.toString()), root2);

    return retVal;
}

static std::string test_json_db_load_one(){
    std::string retVal;
    JsonDb db;

    Json_t read = db.load(tableName, std::make_shared<json::JsonString>(id.toString()));
    if (!read)
        retVal += "\n Could not import saved data";
    else if (read->getType() != JSON_OBJECT)
        retVal += "\n Imported data mismatch";
    else {
        JsonObject* obj = read->toObject();

        if (obj->value.size() != 3)
            retVal += "\n Wrong number of data";

        if (obj->value["id"]->getType() != JSON_STRING)
            retVal += "\n Wrong type for id";
        else if (std::dynamic_pointer_cast<JsonString>(obj->value["id"])->value != id.toString())
            retVal += "\n Wrong id";

        if (obj->value["test1"]->getType() != JSON_BOOL)
            retVal += "\n Wrong type for test1";
        else if (std::dynamic_pointer_cast<JsonBool>(obj->value["test1"])->value != true)
            retVal += "\n Wrong value for test1";

        if (obj->value["test2"]->getType() != JSON_BOOL)
            retVal += "\n Wrong type for test2";
        else if (std::dynamic_pointer_cast<JsonBool>(obj->value["test2"])->value != false)
            retVal += "\n Wrong value for test2";
    }

    return retVal;
}

static std::string test_json_db_load_all(){
    std::string retVal;

    JsonDb db;

    std::vector<Json_t> read = db.loadAll(tableName);
    if (read.size() != 2)
        retVal += "\n Could not import saved data";

    return retVal;
}

static std::string test_json_db_load_idx(){
    std::string retVal;

    JsonDb db;

    Json_t read = db.load(tableName, toJson(id));
    if (!read || read->getType() == JSON_NONE)
        retVal += "\n Could not import saved data";

    read = db.load(tableName, toJson(id));
    if (!read || read->getType() == JSON_NONE)
        retVal += "\n Could not import saved data2";

    return retVal;
}

static std::string test_json_db_load_save(){
    std::string retVal;

    JsonDb db;

    JsonObject_t nO = std::make_shared<JsonObject>();
    nO->value["id"] = std::make_shared<JsonUUID>(UUID::generateUUID());
    nO->value["test1"] = std::make_shared<JsonBool>(true);
    nO->value["test2"] = std::make_shared<JsonBool>(false);
    db.save(tableName, nO->value["id"], nO);

    Json_t read = db.load(tableName, toJson(id));
    if (!read || read->getType() == JSON_NONE)
        retVal += "\n Could not import just saved data";

    return retVal;
}

static std::string test_json_search(){
    std::string retVal;

    JsonDb db;
    db.addIndex(tableName, "id", true, true);
    db.scanIndex();

    auto res = db.loadConditionnal(tableName, Clause::CmpEQ("id", json::toJson(id)));

    if (res.size() != 1)
        retVal += "\n Could not find idx";

    return retVal;
}

static std::string test_json_sort(){
    std::string retVal;

    JsonDb db;
    db.addIndex(tableName, "id");
    db.scanIndex();

    Sort sort("id", true);
    auto res = db.loadConditionnal(tableName, nullptr, sort);

    // FIXME: check result

    return retVal;
}


// FIXME: test search, conditionnal, multiple value, preload.

int main(int argc, char **argv) {
    bool valid = true;
    EXE_TEST(test_json_db_setup());
    EXE_TEST(test_json_db_save());
    EXE_TEST(test_json_db_load_one());
    EXE_TEST(test_json_db_load_all());
    EXE_TEST(test_json_db_load_idx());
    EXE_TEST(test_json_db_load_save());
    EXE_TEST(test_json_search());
    EXE_TEST(test_json_sort());
    return valid ? 0 : -1;
}


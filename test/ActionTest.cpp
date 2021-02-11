/*
 * JsonDbTest.cpp
 *
 *  Created on: Jan 10, 2017
 *      Author: daniel
 */


#include <cstdlib>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <cstdio>
#include <iostream>

#include "../src/wrapper/Session.h"
#include "Test.h"


using namespace elladan::json;
using namespace elladan::jsondb;
using elladan::jsondb::field;


const std::string tableName = "test";

static Session ses;

class AutoGenInt{
public:
    int id = 0;
    std::string name;
    int order = -1;
    bool visible = false;
    template <typename A>
    void persist(A& a) {
        elladan::jsondb::id(a, id, true);
        field(a, order, "order");
        field(a, name, "name");
        field(a, visible, "visible");
    }
};


static std::string test_init(){
    std::string retVal;

    // delete the db;
    std::array<char, 128> buffer;
    std::shared_ptr<FILE> pipe(popen("rm -rf data/", "r"), pclose);
    if (!pipe) 
        retVal = "popen() failed!";
    else {
        while (!feof(pipe.get())) {
            fgets(buffer.data(), 128, pipe.get());
        }
    }

    return retVal;
}


static std::string test_session_map_int(){
    std::string retVal;
    ses.mapClass<AutoGenInt>("int", toJson(0));
    return retVal;
}
static std::string test_add_data_int(){
    std::stringstream retVal;

    AutoGenInt val[4] = {
        {0, "first", 4, true},
        {0, "second", 2, false},
        {0, "third", 3, false},
        {0, "Forth", 1, true},
    };

    int i = 0;
    for (auto& ite : val) {
        ses.save(ite);
        if (ite.id != ++i)
            retVal << "\n" <<  "Expected " << ite.name << " to have the id " << i << " but got " << ite.id; 
    }

    return retVal.str();
}

static std::string test_data_int_count(){
    std::string retVal;

    int nbRow = ses.find<AutoGenInt>().count();
    if (nbRow != 4)
        retVal = "Expected 4 row, got " + std::to_string(nbRow);

    return retVal;
}

static std::string test_data_int_id(){
    std::string retVal;

    AutoGenInt row = ses.find<AutoGenInt>().id(1).findOne();
    if (row.id != 1)
        retVal = "Could not load row at idx 1";

    row = ses.find<AutoGenInt>().id(5).findOne();
    if (row.id != 0)
        retVal = "Loaded invalid value";

    return retVal;
}


static std::string test_data_int_where() {
    std::stringstream retVal;

    int count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::EQ, "/visible", true)).count();
    if (count != 2)
        retVal << "\n" <<  "Expected 2 value with /visible = true. Got " << count;

    auto res = ses.find<AutoGenInt>().where(CmpClause(CmpClause::GE, "/order", 2)).findAll();
    if (res.size() != 3)
        retVal << "\n" <<  "Expected 3 value with /order >= 2. Got " << res.size();
    else {
        for (auto& ite : res) {
            if (ite.order < 1) 
                retVal << "\n" <<  "Expected no order with value < 2. Got " << ite.order;
        }
    }


    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::NE, "/order", 2)).count();
    if (count != 3)
        retVal << "\n" <<  "Expected 3 value with /order != 2. Got " << count;

    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::LT, "/order", 2)).count();
    if (count != 1)
        retVal << "\n" <<  "Expected 1 value with /order < 2. Got " << count;
    
    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::LE, "/order", 2)).count();
    if (count != 2)
        retVal << "\n" <<  "Expected 2 value with /order <= 2. Got " << count;
    
    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::EQ, "/order", 2)).count();
    if (count != 1)
        retVal << "\n" <<  "Expected 1 value with /order == 2. Got " << count;
    
    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::GE, "/order", 2)).count();
    if (count != 3)
        retVal << "\n" <<  "Expected 3 value with /order >= 2. Got " << count;
    
    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::GT, "/order", 2)).count();
    if (count != 2)
        retVal << "\n" <<  "Expected 2 value with /order > 2. Got " << count;



    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::EQ, "/name", "first")).count();
    if (count != 1)
        retVal << "\n" <<  "Expected 1 value with /name == first. Got " << count;

    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::EQ, "/name", "forth", true)).count();
    if (count != 0)
        retVal << "\n" <<  "Expected 0 value with /name == forth. Got " << count;

    count = ses.find<AutoGenInt>().where(CmpClause(CmpClause::EQ, "/name", "Forth", false)).count();
    if (count != 1)
        retVal << "\n" <<  "Expected 1 value with /name == Forth (CI). Got " << count;

    return retVal.str();
}

static std::string test_data_int_relaod() {
    std::stringstream retVal;

    Session db;
    db.mapClass<AutoGenInt>("int", toJson(0));
    db.scanIndex();

    // Fixme: need to work!
    // int total = db.find<AutoGenInt>().id(2).count();
    // if (total != 1)
    //     retVal << "\n" <<  "Reloaded: Expected 1 id with value 2 = true. Got " << total;

    auto ina = db.find<AutoGenInt>().id(2).findOne();
    if (ina.name != "second")
        retVal << "Reloaded: Expected \"second\" item. Got \"" << ina.name << "\"\n" ;

    int count = db.find<AutoGenInt>().where(CmpClause(CmpClause::EQ, "/visible", true)).count();
    if (count != 2)
        retVal << "\n" <<  "Reloaded: Expected 2 value with /visible = true. Got " << count;

    auto res = db.find<AutoGenInt>().where(CmpClause(CmpClause::GE, "/order", 2)).findAll();
    if (res.size() != 3)
        retVal << "\n" <<  "Reloaded: Expected 3 value with /order >= 2. Got " << res.size();
    else
        for (auto& ite : res) {
            if (ite.order <= 1) 
                retVal << "\n" <<  "Reloaded: Expected no order with value < 2. Got " << ite.order;
        }

    return retVal.str();
}




class SelfString{
public:
    std::string id;
    std::string name;
    int order = -1;
    bool visible = false;
    template <typename A>
    void persist(A& a) {
        elladan::jsondb::id(a, id, false);
        field(a, order, "order");
        field(a, name, "name");
        field(a, visible, "visible");
    }
};

static std::string test_session_map_create_str(){
    std::string retVal;

    ses.mapClass<SelfString>("str", Json_t());
    ses.addIndex<SelfString>("/name", true);

    return retVal;
}

static std::string test_add_data_str(){
    std::stringstream retVal;

    SelfString val[4] = {
        {"a", "first",  4, true},
        {"A", "second", 2, false},
        {"b", "third",  3, false},
        {"B", "Forth",  1, true},
    };
    
    for (auto& ite : val)         
        ses.save(ite);

    return retVal.str();
}

static std::string test_data_str_count(){
    std::string retVal;

    int nbRow = ses.find<SelfString>().count();
    if (nbRow != 4)
        retVal = "Expected 4 row, got " + std::to_string(nbRow);

    return retVal;
}

static std::string test_data_str_id(){
    std::string retVal;

    auto row = ses.find<SelfString>().id("b").findOne();
    if (row.name != "third")
        retVal = "Could not load row at idx b";

    row = ses.find<SelfString>().id("A").findOne();
    if (row.name != "second")
        retVal = "Could not load row at idx A";

    row = ses.find<SelfString>().id("c").findOne();
    if (!row.id.empty())
        retVal = "Loaded invalid value";

    return retVal;
}

static std::string test_data_str_where() {
    std::stringstream retVal;

    int count = ses.find<SelfString>().where(CmpClause(CmpClause::EQ, "/visible", true)).count();
    if (count != 2)
        retVal << "\n" <<  "Expected 2 value with /visible = true. Got " << count;

    auto res = ses.find<SelfString>().where(CmpClause(CmpClause::GE, "/order", 2)).findAll();
    if (res.size() != 3)
        retVal << "\n" <<  "Expected 3 value with /order >= 2. Got " << res.size();
    else {
        for (auto& ite : res) {
            if (ite.order < 1) 
                retVal << "\n" <<  "Expected no order with value < 2. Got " << ite.order;
        }
    }

    count = ses.find<SelfString>().where(CmpClause(CmpClause::EQ, "/name", "first")).count();
    if (count != 1)
        retVal << "\n" <<  "Expected 1 value with /name == first. Got " << count;

    count = ses.find<SelfString>().where(CmpClause(CmpClause::EQ, "/name", "forth", true)).count();
    if (count != 0)
        retVal << "\n" <<  "Expected 0 value with /name == forth. Got " << count;

    count = ses.find<SelfString>().where(CmpClause(CmpClause::EQ, "/name", "Forth", false)).count();
    if (count != 1)
        retVal << "\n" <<  "Expected 1 value with /name == Forth ci. Got " << count;

    return retVal.str();
}

static std::string test_data_str_relaod() {
    std::stringstream retVal;

    Session db;
    db.mapClass<SelfString>("str", Json_t());
    db.addIndex<SelfString>("/name", false);
    db.scanIndex();

    auto ina = db.find<SelfString>().id("A").findOne();
    if (ina.name != "second")
        retVal << "Reloaded: Expected \"second\" item. Got \"" << ina.name << "\"\n" ;

    ina = db.find<SelfString>().id("b").findOne();
    if (ina.name != "third")
        retVal << "Reloaded: Expected \"second\" item. Got \"" << ina.name << "\"\n" ;

    int count = db.find<SelfString>().where(CmpClause(CmpClause::EQ, "/visible", true)).count();
    if (count != 2)
        retVal << "\n" <<  "Reloaded: Expected 2 value with /visible = true. Got " << count;

    auto res = db.find<SelfString>().where(CmpClause(CmpClause::GE, "/order", 2)).findAll();
    if (res.size() != 3)
        retVal << "\n" <<  "Reloaded: Expected 3 value with /order >= 2. Got " << res.size();
    else
        for (auto& ite : res) {
            if (ite.order <= 1) 
                retVal << "\n" <<  "Reloaded: Expected no order with value < 2. Got " << ite.order;
        }

    return retVal.str();
}





class SelfStringCIS{
public:
    std::string id;
    std::string name;
    int order;
    bool visible;
    template <typename A>
    void persist(A& a) {
        elladan::jsondb::id(a, id, false);
        field(a, order, "order");
        field(a, name, "name");
        field(a, visible, "visible");
    }
};


static std::string test_session_map_create_ci(){
    std::string retVal;

    ses.mapClass<SelfStringCIS>("ci", Json_t());
    ses.addIndex<SelfStringCIS>("/name", false);
    
    return retVal;
}

static std::string test_add_data_ci(){
    std::stringstream retVal;

    SelfStringCIS val[4] = {
        {"a", "first",  4, true},
        {"b", "second", 2, false},
        {"c", "third",  3, false},
        {"C", "forth",  1, true},
    };

    for (auto& ite : val)         
        ses.save(ite);

    return retVal.str();
}

static std::string test_data_ci_count() {
    std::string retVal;

    int nbRow = ses.find<SelfStringCIS>().count();
    if (nbRow != 3)
        retVal = "Expected 3 row, got " + std::to_string(nbRow);

    return retVal;
}

static std::string test_data_ci_id() {
    std::string retVal;

    auto row = ses.find<SelfStringCIS>().id("b").findOne();
    if (row.name != "second")
        retVal = "Could not load row at idx b";

    row = ses.find<SelfStringCIS>().id("A").findOne();
    if (row.name != "first")
        retVal = "Could not load row at idx A";

    row = ses.find<SelfStringCIS>().id("c").findOne();
    if (!row.id.empty())
        retVal = "Loaded invalid value";

    return retVal;
}

static std::string test_data_ci_where() {
    std::stringstream retVal;

    int count = ses.find<SelfStringCIS>().where(CmpClause(CmpClause::EQ, "/visible", true)).count();
    if (count != 2)
        retVal << "\n" <<  "Expected 2 value with /visible = true. Got " << count;

    auto res = ses.find<SelfStringCIS>().where(CmpClause(CmpClause::GE, "/order", 2)).findAll();
    if (res.size() != 3)
        retVal << "\n" <<  "Expected 3 value with /order >= 2. Got " << res.size();
    else {
        for (auto& ite : res) {
            if (ite.order < 1) 
                retVal << "\n" <<  "Expected no order with value < 2. Got " << ite.order;
        }
    }

    count = ses.find<SelfStringCIS>().where(CmpClause(CmpClause::EQ, "/name", "first")).count();
    if (count != 1)
        retVal << "\n" <<  "Expected 1 value with /name == first. Got " << count;

    count = ses.find<SelfStringCIS>().where(CmpClause(CmpClause::EQ, "/name", "forth", true)).count();
    if (count != 0)
        retVal << "\n" <<  "Expected 0 value with /name == forth. Got " << count;

    count = ses.find<SelfStringCIS>().where(CmpClause(CmpClause::EQ, "/name", "Forth", false)).count();
    if (count != 1)
        retVal << "\n" <<  "Expected 1 value with /name == Forth ci. Got " << count;

    return retVal.str();
}

static std::string test_data_ci_relaod() {
    std::stringstream retVal;

    Session db;
    db.mapClass<SelfStringCIS>("ci", Json_t());
    db.addIndex<SelfStringCIS>("/name", true);
    db.scanIndex();

    auto ina = db.find<SelfStringCIS>().id("A").findOne();
    if (ina.name != "first")
        retVal << "Reloaded: Expected \"first\" item. Got \"" << ina.name << "\"\n" ;

    ina = db.find<SelfStringCIS>().id("b").findOne();
    if (ina.name != "second")
        retVal << "Reloaded: Expected \"second\" item. Got \"" << ina.name << "\"\n" ;

    int count = db.find<SelfStringCIS>().where(CmpClause(CmpClause::EQ, "/visible", true)).count();
    if (count != 2)
        retVal << "\n" <<  "Reloaded: Expected 2 value with /visible = true. Got " << count;

    auto res = db.find<SelfStringCIS>().where(CmpClause(CmpClause::GE, "/order", 2)).findAll();
    if (res.size() != 2)
        retVal << "\n" <<  "Reloaded: Expected 2 value with /order >= 2. Got " << res.size();
    else
        for (auto& ite : res) {
            if (ite.order <= 1) 
                retVal << "\n" <<  "Reloaded: Expected no order with value < 2. Got " << ite.order;
        }

    return retVal.str();
}


int main(int argc, char **argv) {
    bool valid = true;
    EXE_TEST(test_init());

    EXE_TEST(test_session_map_int());
    EXE_TEST(test_add_data_int());
    EXE_TEST(test_data_int_count());
    EXE_TEST(test_data_int_id());
    EXE_TEST(test_data_int_where());
    EXE_TEST(test_data_int_relaod());


    EXE_TEST(test_session_map_create_str());
    EXE_TEST(test_add_data_str());
    EXE_TEST(test_data_str_count());
    EXE_TEST(test_data_str_id());
    EXE_TEST(test_data_str_where());
    EXE_TEST(test_data_str_relaod());


    // EXE_TEST(test_session_map_create_ci());
    // EXE_TEST(test_add_data_ci());
    // EXE_TEST(test_data_ci_count());
    // EXE_TEST(test_data_ci_id());
    // EXE_TEST(test_data_ci_where());
    // EXE_TEST(test_data_ci_relaod());

    return valid ? 0 : -1;
}

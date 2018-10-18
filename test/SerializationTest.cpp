/*
 * SFINARTest.cpp
 *
 *  Created on: Oct 5, 2017
 *      Author: daniel
 */

#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <vector>


#include <cstdio>
#include <string>

#include "../src/wrapper/Action.h"
#include "../src/wrapper/Session.h"

using namespace elladan::json;
using namespace elladan::jsondb;


#define EXE_TEST(func) do { std::string str = func; if (!str.empty()){ valid = false; printf("Test " #func  " failed : %s\n", str.c_str());} } while (0)

class Simple{
public:
    template <typename A>
    void persist(A& a) {
        field(a, val, "val");
    }

    Simple () {val=0;}
    Simple (int v) {val=v;}

    bool operator< (const Simple& rhs) const {
        return val < rhs.val;
    }

    int val;
};

class Complex{
public:
    template <typename A>
    void persist(A& a) {
        field(a, s , "s");
        field(a, i , "i");
        field(a, ss, "ss");
        field(a, si, "si");
        field(a, vs, "vs");
        field(a, vi, "vi");
        field(a, ms, "ms");
        field(a, mi, "mi");
        field(a, vms, "vms");
        field(a, vmi, "vmi");
    }

    Simple s;
    int i;
    std::set<Simple> ss;
    std::set<int> si;
    std::vector<Simple> vs;
    std::vector<int> vi;
    std::map<std::string, Simple> ms;
    std::map<std::string, int> mi;
    std::map<std::string, Simple> vms;
    std::map<std::string, int> vmi;
};

static std::string test_simple(){
    std::string retVal;

    Simple input, output;
    input.val = 3;

    WriteAction wr(std::make_shared<JsonObject>());
    try {
        input.persist(wr);

        JsonObject* obj = wr.doc->toObject();
        if (obj->value["val"]->getType() != JSON_INTEGER)
            retVal += "\n Did not serialize simple value ";
        else if (obj->value["val"]->toInt()->value != 3)
            retVal += "\n Simple value is wrong, expected \"3\" got " + std::to_string(wr.doc->toObject()->value["val"]->toInt()->value);
    }
    catch (std::exception& e) {
        retVal += "\n Got exception while serializing Simple class : ";
        retVal += e.what();
    }

    if (retVal.empty()) {
        try {
            ReadAction ra;
            ra.doc = std::dynamic_pointer_cast<JsonObject>(wr.doc);
            output.persist(ra);

            if (output.val != 3)
                retVal += "\n Simple value is read wrongly, expected \"3\" got " + std::to_string(output.val);

        }
        catch (std::exception& e) {
            retVal += "\n Got exception while deserializing Simple class : ";
            retVal += e.what();
        }
    }

    return retVal;
}


static std::string test_complex(){
    std::string retVal;

    Complex input, output;
    input.i = 0;
    input.s.val = 1;

    input.si.insert(2);
    input.si.insert(3);
    input.si.insert(4);
    input.ss.insert(Simple(5));
    input.ss.insert(Simple(6));
    input.ss.insert(Simple(7));
    input.vi.push_back(8);
    input.vi.push_back(9);
    input.vi.push_back(10);
    input.vs.push_back(Simple(11));
    input.vs.push_back(Simple(12));
    input.vs.push_back(Simple(13));
    input.mi["0"] = 8;
    input.mi["1"] = 9;
    input.mi["2"] = 10;
    input.ms["3"] = Simple(11);
    input.ms["4"] = Simple(12);
    input.ms["5"] = Simple(13);
    input.vmi["7"] = 14;
    input.vmi["8"] = 15;
    input.vmi["9"] = 16;
    input.vms["10"] = Simple(17);
    input.vms["11"] = Simple(18);
    input.vms["12"] = Simple(19);

    // FIXME: update for complex.
    WriteAction wr(std::make_shared<JsonObject>());
    try{
        input.persist(wr);

        auto res = wr.doc->getChild(wr.doc, "/s/val");
        if (res.empty() || res.front()->getType() != JSON_INTEGER)
            retVal += "\n Did not serialize complex value ";
        else if (res.front()->toInt()->value != 1)
            retVal += "\n complex value is wrong, expected \"1\" got " + std::to_string(res.front()->toInt()->value);
    }
    catch (std::exception& e) {
        retVal += "\n Got exception while serializing complex class : ";
        retVal += e.what();
    }

    if (retVal.empty()) {
        try{
            ReadAction ra;
            ra.doc = std::dynamic_pointer_cast<JsonObject>(wr.doc);
            output.persist(ra);

            if (output.s.val != 1)
                retVal += "\n complex value is read wrongly, expected \"1\" got " + std::to_string(output.s.val);

        }
        catch (std::exception& e) {
            retVal += "\n Got exception while deserializing complex class : ";
            retVal += e.what();
        }
    }

    return retVal;
}

class ID_i{
public:
    ID_i() : i(0) {}
    template <typename A>
    void persist(A& a) {
        id(a, i, true);
    }
    int i;
};
class ID_s{
public:
    template <typename A>
    void persist(A& a) {
        id(a, s, true);
    }
    std::string s;
};
class ID_u{
public:
    template <typename A>
    void persist(A& a) {
        id(a, u, true);
    }
    elladan::UUID u;
};

std::string test_autoId(){
    std::string retVal;

    Session ses;

    ses.mapClass<ID_i>("int", elladan::json::toJson(0));
    int last = 0;
    for (int i = 0; i < 100; i++) {
        ID_i v;
        ses.save(v);
        if (v.i == 0)
            retVal += "\nInvalid int id, value not set";
        else if (v.i <= last)
            retVal += "\nInvalid new id, value already in use";
        else continue;

        last = v.i;

        break;
    }
    if (ses.find<ID_i>().count() != 100)
        retVal += "\nInvalid int id, not all created (likely overwritten)";

    ses.mapClass<ID_s>("string", elladan::json::toJson(""));
    for (int i = 0; i < 100; i++) {
        ID_s v;
        ses.save(v);
        if (v.s.size() == 0) {
            retVal += "\nInvalid string id, value not set";
            break;
        }
    }
    if (ses.find<ID_s>().count() != 100)
        retVal += "\nInvalid string id, not all created (likely overwritten)";

    ses.mapClass<ID_u>("uuid", elladan::json::toJson(elladan::UUID()));
    for (int i = 0; i < 100; i++) {
        ID_u v;
        ses.save(v);
        if (v.u.cmp(elladan::UUID()) == 0) {
            retVal += "\nInvalid uuid id, value not set";
            break;
        }
    }
    if (ses.find<ID_u>().count() != 100)
        retVal += "\nInvalid uuid id, not all created (likely overwritten)";

    return retVal;

}


int main(int argc, char **argv) {
    bool valid = true;
    EXE_TEST(test_simple());
    EXE_TEST(test_complex());
    EXE_TEST(test_autoId());
    return valid ? 0 : -1;
}


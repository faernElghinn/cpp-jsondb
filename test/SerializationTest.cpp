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
    }

    Simple s;
    int i;
    std::set<Simple> ss;
    std::set<int> si;
    std::vector<Simple> vs;
    std::vector<int> vi;
    std::map<std::string, Simple> ms;
    std::map<std::string, int> mi;
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

    // FIXME: update for complex.
    WriteAction wr(std::make_shared<JsonObject>());
    try{
        input.persist(wr);

        auto res = wr.doc->getChild(wr.doc, "s/val");
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

int main(int argc, char **argv) {
    bool valid = true;
    EXE_TEST(test_simple());
    EXE_TEST(test_complex());
    return valid ? 0 : -1;
}


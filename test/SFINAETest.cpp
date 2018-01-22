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


class Empty{
public:
    template <typename A>
    void persist(A& a) {}
};


static std::string test_SFINAE(){
    std::string retVal;

    if (is_vector<int>::value) retVal += "\n int mapped as an vector";
    if (is_map<int>::value) retVal += "\n int mapped as an map";
    if (is_set<int>::value) retVal += "\n int mapped as an set";
    if (has_persist<int>::value) retVal += "\n int mapped as has persist";

    if (is_vector<std::map<int,int>>::value) retVal += "\n map mapped as an vector";
    if (!is_map<std::map<int,int>>::value) retVal += "\n map is not mapped as an map";
    if (is_set<std::map<int,int>>::value) retVal += "\n map mapped as an set";
    if (has_persist<std::map<int,int>>::value) retVal += "\n map mapped as has persist";

    if (is_vector<std::set<int>>::value) retVal += "\n set mapped as an vector";
    if (is_map<std::set<int>>::value) retVal += "\n set mapped as an map";
    if (!is_set<std::set<int>>::value) retVal += "\n set not mapped as an set";
    if (has_persist<std::set<int>>::value) retVal += "\n set mapped as has persist";

    if (!is_vector<std::vector<int>>::value) retVal += "\n vector not mapped as an vector";
    if (is_map<std::vector<int>>::value) retVal += "\n vector mapped as an map";
    if (is_set<std::vector<int>>::value) retVal += "\n vector mapped as an set";
    if (has_persist<std::vector<int>>::value) retVal += "\n vector mapped as has persist";

    if (is_vector<Empty>::value) retVal += "\n Bidon mapped as an vector";
    if (is_map<Empty>::value) retVal += "\n Bidon mapped as an map";
    if (is_set<Empty>::value) retVal += "\n Bidon mapped as an set";
    if (!has_persist<Empty>::value) retVal += "\n Bidon not mapped as has persist";

    return retVal;
}



int main(int argc, char **argv) {
    bool valid = true;
    EXE_TEST(test_SFINAE());
    return valid ? 0 : -1;
}


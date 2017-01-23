/*
 * JsonDbTest.cpp
 *
 *  Created on: Jan 10, 2017
 *      Author: daniel
 */

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "../src/db/FileManipulator.h"
#include "../src/db/Random.h"
#include "../src/db/UUID.h"

namespace db {
class UUID;
} /* namespace db */

namespace db {
namespace test {

static void test_random(){
    printf("Random \n");

    unsigned int r = db::Random::get(1000);
    assert ((r < 1000));

    unsigned int r2 = db::Random::get(1000);
    assert (r2 != r);

    std::string str = db::Random::generateString(64);
    assert(str.size() == 64);
    for (auto ite : str)
        assert ((ite >= 'a' && ite <= 'z') ||
                (ite >= 'A' && ite <= 'Z') ||
                (ite >= '0' && ite <= '9') );

    printf("Random - DONE \n");
}

static void test_uuid(){
    printf("UUID \n");

    db::UUID udBlank;
    db::UUID ud1;
    db::UUID ud2;
    assert(ud1 == ud2); // Empty uuid must match.

    ud1 = db::UUID::generateUUID();
    ud2 = db::UUID::generateUUID();
    assert(ud1 != udBlank); // Random uuid should not be blank.
    assert(ud1 != ud2); // Random uuid should not match.

    ud2 = db::UUID::fromString(ud1.toString());
    assert (ud1 == ud2); // Validate exporting and re-importing value,
    printf("UUID - DONE \n");
}

static void test_string_manip(){
    printf("String Manipulation \n");

    auto token = tokenize("/1/2/3//5", "/");
    assert(token.size() == 6);
    assert(token[0].empty());
    assert(token[1] == "1");
    assert(token[2] == "2");
    assert(token[3] == "3");
    assert(token[4].empty());
    assert(token[5] == "5");

    std::string entry = "/path/with/many//slashee//";
    std::string expected = "/path/with/many/slashee/";
    assert(removeDoubleSlash(entry) == expected);

    printf("String Manipulation - DONE \n");
}

static void test_folder_manip(){
    printf("Folder Manipulation \n");

    system( "rm -rf /tmp/JsonUnitTest" ); // Clean previous test.
    createFolder("/tmp/JsonUnitTest/child/subchild"); // Create the folder hierarchy.
    assert(system( "touch /tmp/JsonUnitTest/child/subchild/1.json") == 0); // Touch fail if parent don't exist.
    assert(listJsonFile("/tmp/JsonUnitTest/child/subchild/").size() == 1);
    system( "touch /tmp/JsonUnitTest/child/subchild/2.json");
    system( "touch /tmp/JsonUnitTest/child/subchild/3.json");
    system( "touch /tmp/JsonUnitTest/child/subchild/4.json");
    system( "touch /tmp/JsonUnitTest/child/subchild/5.txt");
    auto files = listJsonFile("/tmp/JsonUnitTest/child/subchild/");

    bool found = false;
    for (auto& ite : files)
        found |= ite == "1.json";
    assert(found); // Test that the found filename are valid

    found = false;
    for (auto& ite : files)
        found |= ite == "5.txt";
    assert(!found); // Make sure we have only json file

    system( "rm -rf /tmp/JsonUnitTest" ); // Cleanup
    printf("Folder Manipulation - DONE \n");
}

int main(int argc, char * argv[]) {
    test_random();
    test_uuid();
    test_string_manip();
    test_folder_manip();
}


} /* namespace test */
} /* namespace db */

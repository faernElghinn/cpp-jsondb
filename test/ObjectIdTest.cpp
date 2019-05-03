#include "../src/ObjectId.h"

#include <cstring>
#include <chrono>

#include "Test.h"

using std::to_string;

static std::string test_creation(){
    std::string retVal;

    ObjectId id;
    if (memcmp(id.data, "\0\0\0\0\0\0\0\0\0\0\0\0", 12) != 0) {
       retVal = "\nExpected null ObjectId";
    }

    auto now = (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())).count();
    auto id0 = ObjectId::generate();
    auto id1 = ObjectId::generate();

    if (id0 == id1) {
       retVal = "\nExpected different ObjectId, got " + to_string(id0) + " and " + to_string(id1);
    }

    if (std::abs(id0.epoch() - id1.epoch())> 1) {
      retVal = "\nShoild be near each other, got " + to_string(id0.epoch()) + " and " + to_string(id1.epoch());
    }

    if (std::abs(id0.epoch() - now) > 1) {
      retVal = "\nShould be near now, got " + to_string(id0.epoch()) + " and " + to_string(now);
    }

    return retVal;
}

static std::string test_serialization(){
    std::string retVal;

   auto id0 = ObjectId::generate();
   auto id1 = ObjectId(id0.toString());

   if (id1 != id0) {
      retVal = "\nCould not deserialize and re-serialize";
   }

   return retVal;
}

// FIXME: test search, conditional, multiple value, preload.

int main(int argc, char **argv) {
    bool valid = true;
    EXE_TEST(test_creation());
    EXE_TEST(test_serialization());
    return valid ? 0 : -1;
}
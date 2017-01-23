/*
 * Random.h
 *
 *  Created on: Aug 31, 2016
 *      Author: daniel
 */

#include "Random.h"

namespace db {

::std::random_device Random::_src;

unsigned int Random::get(int max){
    ::std::uniform_int_distribution<int> dist(0, max);
    return dist(_src);
}
::std::string Random::generateString(int length){
    static const char seq_start[] = {'0', 'A'-'0', 'a'-'A'};
    ::std::string result;
    ::std::uniform_int_distribution<int> dist(0, 2 * 26 + 10);

    while (length--) {
        int r = dist(_src);
        int offset = r < 10 ? 0 : (r < 36 ? 1 : 2);
        result.push_back(r+seq_start[offset]);
    }

    return result;
}


} /* namespace db */


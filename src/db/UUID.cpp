/*
 * Random.h
 *
 *  Created on: Aug 31, 2016
 *      Author: daniel
 */

#include "UUID.h"
#include <cstdio>

#include "Random.h"

namespace db {

UUID UUID::generateUUID(){
    UUID uuid;
    for (auto && ite : uuid._uuid)
        ite = Random::get(UINT16_MAX);
    return uuid;
}

UUID::UUID() { memset(_uuid, 0, sizeof(_uuid)); }

bool UUID::operator!(){
    uint16_t mask = 0;
    for (auto ite : _uuid)
        mask |= ite;
    return !mask;
}

std::string UUID::toString() const {
    char data[] = "00000000-0000-0000-0000-000000000000";
    sprintf(data, "%0X%0X-%0X-%0X-%0X-%0X%0X%0X",
            _uuid[0], _uuid[1], _uuid[2], _uuid[3],
            _uuid[4], _uuid[5], _uuid[6], _uuid[7]);
    return data;
}

UUID UUID::fromString(const std::string& str){
    int toto[8];
    memset(toto, 0, sizeof(toto));

    int nbVal = sscanf(str.c_str(), "%2X%2X-%2X-%2X-%2X-%2X%2X%2X",
            toto+0, toto+1, toto+2, toto+3,
            toto+4, toto+5, toto+6, toto+7);

    UUID id;
    if (nbVal == 8)
        for (int i = 0; i < 8; i++)
            id._uuid[i] = toto[8];

    return id;
}

std::string toString(const db::UUID& ele){
    return ele.toString();
}

} /* namespace db */

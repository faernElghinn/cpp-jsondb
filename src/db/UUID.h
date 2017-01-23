/*
 * Random.h
 *
 *  Created on: Aug 31, 2016
 *      Author: daniel
 */

#ifndef SRC_UUID_H_
#define SRC_UUID_H_

#include <cstdint>
#include <cstring>
#include <string>

namespace db {

class UUID
{
public:
    static UUID generateUUID();

    UUID();

    UUID(const UUID& other) {
        memcpy(_uuid, other._uuid, sizeof(_uuid));
    }

    bool operator!();

    inline bool operator==(const UUID& other) {
        return std::memcmp(_uuid, other._uuid, sizeof(_uuid)) == 0;
    }
    inline bool operator!=(const UUID& other) {
        return std::memcmp(_uuid, other._uuid, sizeof(_uuid)) != 0;
    }
    inline bool operator>(const UUID& other) {
        return std::memcmp(_uuid, other._uuid, sizeof(_uuid)) > 0;
    }
    inline bool operator<(const UUID& other) {
        return std::memcmp(_uuid, other._uuid, sizeof(_uuid)) < 0;
    }

    std::string toString() const;
    static UUID fromString(const std::string& str);

private:
    uint16_t _uuid[8];

};

std::string toString(const db::UUID& ele);

} /* namespace db */

#endif /* SRC_UUID_H_ */

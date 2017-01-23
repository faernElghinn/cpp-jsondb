/*
 * JsonException2.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#include "JsonException.h"

namespace db {
namespace json {

JsonException::JsonException(std::string str) :
            std::runtime_error(str) {
    }

} /* namespace json */
} /* namespace db */

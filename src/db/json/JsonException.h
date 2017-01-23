/*
 * JsonException2.h
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#ifndef SRC_DB_JSON_JSONEXCEPTION_H_
#define SRC_DB_JSON_JSONEXCEPTION_H_

#include <stdexcept>
#include <string>

namespace db {
namespace json {

class JsonException: public std::runtime_error
{
public:
    JsonException(std::string str);
};


} /* namespace json */
} /* namespace db */

#endif /* SRC_DB_JSON_JSONEXCEPTION_H_ */

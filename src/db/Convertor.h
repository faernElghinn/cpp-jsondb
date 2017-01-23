/*
 * Convertor.h
 *
 *  Created on: Jan 19, 2017
 *      Author: daniel
 */

#ifndef SRC_DB_CONVERTOR_H_
#define SRC_DB_CONVERTOR_H_

#include <string>

namespace db {

std::string toString(const std::string& val);
std::string toString(int val);
std::string toString(float val);
std::string toString(bool val);
std::string toString(char val);


} /* namespace db */

#endif /* SRC_DB_CONVERTOR_H_ */

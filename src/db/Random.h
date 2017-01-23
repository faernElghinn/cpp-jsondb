/*
 * Random.h
 *
 *  Created on: Aug 31, 2016
 *      Author: daniel
 */

#ifndef SRC_RANDOM_H_
#define SRC_RANDOM_H_

#include <string>

#include <random>

namespace db {

class Random
{
public:
  static unsigned int get(int max);
  static ::std::string generateString(int length = 32);
private:
  static ::std::random_device _src;
};


} /* namespace db */

#endif /* SRC_UUID_H_ */

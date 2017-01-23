/*
 * Convertor.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: daniel
 */

#include "Convertor.h"

namespace db {



std::string toString(const std::string& val)    {return val;}
std::string toString(int val)                   {return std::to_string(val);}
std::string toString(float val)                 {return std::to_string(val);}
std::string toString(char val)                  {return std::to_string(val);}

std::string toString(bool val)                  {
    if (val) return "true";
    return "false";
}


} /* namespace db */

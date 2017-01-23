/*
 * ReadStatement.cpp
 *
 *  Created on: Jan 13, 2017
 *      Author: daniel
 */

#include <string>
#include "ReadStatement.h"

namespace db {
namespace json {

template <>
bool CmpClause<std::string>::testRelation(std::string value){
    switch (_mode) {
        case LT:   return value.compare(_expected)  < 0;
        case LE:   return value.compare(_expected) <= 0;
        case GE:   return value.compare(_expected) >= 0;
        case GT:   return value.compare(_expected)  > 0;
        default: return false;
    }
}


#define GetValueType(Type, Name) template<> bool getValue<Type>(JsonElement& ele, Type& val) { if (!ele.is##Name()) return false; val = ele.asValue().get##Name(); return true;}
GetValueType(bool, Bool)
GetValueType(int, Int)
GetValueType(long, Int)
GetValueType(unsigned, Int)
GetValueType(float, Float)
GetValueType(double, Float)
GetValueType(std::string, String)
#undef GetValueType

} /* namespace json */
} /* namespace db */

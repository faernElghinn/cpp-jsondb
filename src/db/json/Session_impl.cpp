/*
 * Session_impl.cpp
 *
 *  Created on: Jan 9, 2017
 *      Author: daniel
 */


#include "Session_impl.h"

namespace db {
namespace json {

SessionImp::SessionImp(){}
SessionImp::~SessionImp(){}

void SessionImp::getLock(const std::string& filename){
    std::unique_lock<std::mutex> guard(_lock);

    while (_fileLock.count(filename))
    {
        _cond.wait(guard);
    }
    _fileLock.insert(filename);
}
void SessionImp::releaseLock(const std::string& filename){
    {
        std::lock_guard<std::mutex> guard(_lock);
        _fileLock.erase(filename);
    }
    _cond.notify_all();
}

} /* namespace json */
} /* namespace db */

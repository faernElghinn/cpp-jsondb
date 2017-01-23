/*
 * Controller.h

 *
 *  Created on: Dec 21, 2016
 *      Author: daniel
 */

#ifndef SRC_DB_JSON_MANAGER_SESSION_IMPL_H_
#define SRC_DB_JSON_MANAGER_SESSION_IMPL_H_

#include <stddef.h>
#include <cassert>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <typeinfo>

#include "../FileManipulator.h"
#include "Action.h"
#include "Json.h"
#include "ReadStatement.h"

/*
 * Logic:
 * Each transaction is done in part:
 * 1- Prepare transaction
 *    - If read, get the what (and optionnaly filters/sort)
 *    - If write,
 *      - If single object, get the json data and id.
 *      - If multi object (replace all), get query statement.
 * 2- Execute query
 *    - If read query
 *      - Find the file
 *      - If not found : return null obj.
 *      - Await lock of file and lock it
 *      - Get root JsonObject
 *      - Unlock file (let the file system keep it alive)
 *    - If write query
 *      - Find the file
 *        - If not found : create it.
 *      - Await lock of file and lock it
 *      - If already exist : do a backup.
 *      - Write new file.
 *        - On success : delete back up
 *        - On failure : restore backup and throw error.
 *      - Free lock
 */

namespace db {
namespace json {


/// This is the "brain" of the db. It hold the class names and locks.
class SessionImp
{
public:
    SessionImp();
    ~SessionImp();

    // Lock a file. Good only for inter-thread lock.
    void getLock(const std::string& filename);
    // Release a previously locked file.
    void releaseLock(const std::string& filename);

    std::set<std::string> _fileLock;
    std::map<size_t, std::string> _className;

    std::mutex _lock;
    std::condition_variable _cond;
    std::string _basePath;
};

} /* namespace json */
} /* namespace db */

#endif /* SRC_DB_JSON_MANAGER_SESSION_IMPL_H_ */

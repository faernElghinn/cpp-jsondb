/*
 * TransactionReadFile.h
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#ifndef SRC_DB_JSON_MANAGER_TRANSACTIONREADFILE_H_
#define SRC_DB_JSON_MANAGER_TRANSACTIONREADFILE_H_

#include <cstdio>
#include <string>

namespace db {
namespace json {
class SessionImp;
} /* namespace json */
} /* namespace db */

namespace db {
namespace json {

/// Manage access to a file in read only mode.
class TransactionReadFile
{
public:
    TransactionReadFile(SessionImp& ctl);
    TransactionReadFile(const std::string& filename, SessionImp& ctl);
    ~TransactionReadFile();

    // Open a file for ready. Manage element locking to make sure it's not being edited while we are reading it.
    void open(const std::string& filename);

    // Clean handle and lock.
    void close();

    // Do we have a file open?
    inline bool operator!() {
        return _file == nullptr;
    }

    // Get the file handle.
    inline FILE* getFile() {
        return _file;
    }

private:
    std::string _filename;
    SessionImp& _ctl;
    FILE* _file;
};

} /* namespace json */
} /* namespace db */

#endif /* SRC_DB_JSON_MANAGER_TRANSACTIONREADFILE_H_ */

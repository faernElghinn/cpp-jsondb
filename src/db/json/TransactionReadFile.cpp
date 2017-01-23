/*
 * TransactionReadFile.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#include "TransactionReadFile.h"

#include <sys/file.h>

#include "../FileManipulator.h"
#include "JsonException.h"
#include "ReadStatement.h"
#include "Session_impl.h"

namespace db {
namespace json {


TransactionReadFile::TransactionReadFile(SessionImp& ctl) :
                _ctl(ctl), _file(nullptr)
{}
TransactionReadFile::TransactionReadFile(const std::string& filename, SessionImp& ctl) :
                TransactionReadFile(ctl)
{
    open(filename);
}

TransactionReadFile::~TransactionReadFile() {close();}


void TransactionReadFile::open(const std::string& filename) {
    _filename = filename;

    // Make sure we are the only one handling the file in the process.
    _ctl.getLock(_filename);

    // Open a new file.
    _file = fopen(filename.c_str(), "r");
    checkForFileError("open " + _filename + " for writting");
    // It's ok not to exist.

    // Prevent other process from editing the file.
    if (ENABLE_PROCESS_LOCK) {
        if (flock(fileno(_file), LOCK_EX))
            throw db::json::JsonException("Could not lock " + _filename + " for writing.");
    }

    // Now that we known nobody is currently editing the file, release the lock on the file.
    // This work because file are not edited in place, but moved and recreated.

    // Unlock inter-process lock
    if (ENABLE_PROCESS_LOCK)
        flock(fileno(_file), LOCK_UN);

    // Unlock inter-thread lock.
    _ctl.releaseLock(_filename);
}

void TransactionReadFile::close() {
    // Close the file.
    if (_file) {
        fclose(_file);
        _file = nullptr;
    }
}

} /* namespace json */
} /* namespace db */

/*
 * TransactionFile.cpp
 *
 *  Created on: Feb 21, 2017
 *      Author: daniel
 */

#include "TransactionWriteFile.h"

#include <elladan/FileManipulator.h>
#include <elladan/Exception.h>
#include <cerrno>
#include <cstdio>

#include "JsonDb.h"


namespace elladan {
namespace jsondb {


TransactionWriteFile::TransactionWriteFile(const std::string& filename, JsonDb& ctl) :
                        _ctl(ctl), _file(nullptr)
{
    open(filename);
}
TransactionWriteFile::~TransactionWriteFile() {close();}

void TransactionWriteFile::open(const std::string& filename) {
    _filename = filename;

    // Make sure we are the only one handling the file in the process.
    _ctl.getLock(_filename);

    // Make a backup of the file.
    if (rename(filename.c_str(), (_filename+"~").c_str()) == -1)
        checkForFileError("backup " + _filename, errno);

    // Open a new file.
    _file.open(filename, std::ios::out | std::ios::binary);
    if (!_file.is_open())
        throw Exception("Could not open " + _filename + " for writing : File don't exist.");
}

void TransactionWriteFile::rollbackAndClose() {
    closeFile();

    // Delete wrong file.
    remove(_filename.c_str());

    // Restore backup.
    rename((_filename+"~").c_str(), _filename.c_str());

    freeLock();
}

void TransactionWriteFile::close() {
    closeFile();
    freeLock();
}

void TransactionWriteFile::closeFile() {
    _file.close();
}

void TransactionWriteFile::freeLock() {
    if (!_filename.empty()) {
        // Delete backup.
        remove((_filename+"~").c_str());

        _ctl.releaseLock(_filename);
        _filename.clear();
    }
}

} } // namespace ellsadan::jsondb

/*
 * TransactionReadFile.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#include "TransactionReadFile.h"

#include "JsonDb.h"

namespace elladan {
namespace jsondb {

TransactionReadFile::TransactionReadFile(JsonDb& ctl) :
                _ctl(ctl), _file(nullptr)
{}
TransactionReadFile::TransactionReadFile(const std::string& filename, JsonDb& ctl) :
                TransactionReadFile(ctl)
{
    open(ctl.getBasePath() + filename);
}

TransactionReadFile::~TransactionReadFile() {close();}


void TransactionReadFile::open(const std::string& filename) {
    _filename = filename;

    // Make sure we are the only one handling the file in the process.
    _ctl.getLock(_filename);

    // Open a new file.
    _file.open(filename.c_str(), std::ios::in|std::ios::binary);

    //FIXME check for exception.
    // It's ok not to exist.

    // Now that we known nobody is currently editing the file, release the lock on the file.
    // This work because file are not edited in place, but moved and recreated.
    _ctl.releaseLock(_filename);
}

void TransactionReadFile::close() {
    // Close the file.
    _file.close();
}

} } // namespace elladan::jsondb

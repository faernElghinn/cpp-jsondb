/*
 * TransactionFile.h
 *
 *  Created on: Feb 21, 2017
 *      Author: daniel
 */

#pragma once

#include <fstream>
#include <string>

namespace elladan {
namespace jsondb {

class JsonDb;

class TransactionWriteFile
{
public:
    TransactionWriteFile(const std::string& filename, JsonDb& ctl);
    ~TransactionWriteFile();

    // Open file for writing. It also create a backup.
    void open(const std::string& filename);

    // Cancel write and restore element from backup.
    void rollbackAndClose();

    // Release lock and handle.
    void close();

    // Do we have a file open?
    inline bool operator!() {
        return _file.is_open();
    }

    // Get the file handle.
    inline std::ostream& getFile() {
        return _file;
    }

private:
    void closeFile();
    void freeLock();

    std::string _filename;
    JsonDb& _ctl;
    std::ofstream _file;
};


} } // namespace elladan::jsondb


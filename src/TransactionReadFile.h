/*
 * TransactionReadFile.h
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#pragma once

#include <fstream>
#include <string>

namespace elladan {
namespace jsondb {

class JsonDb;

/// Manage access to a file in read only mode.
class TransactionReadFile
{
public:
    TransactionReadFile(JsonDb& ctl);
    TransactionReadFile(const std::string& filename, JsonDb& ctl);
    ~TransactionReadFile();

    // Open a file for ready. Manage element locking to make sure it's not being edited while we are reading it.
    void open(const std::string& filename);

    // Clean handle and lock.
    void close();

    // Do we have a file open?
    inline bool operator!() {
        return !_file.is_open();
    }

    // Get the file handle.
    inline std::ifstream& getFile() {
        return _file;
    }

    inline const std::string& getFileName(){
        return _filename;
    }

protected:
    std::string _filename;
    JsonDb& _ctl;
    std::ifstream _file;
};

} } // namespace elladan::jsondb


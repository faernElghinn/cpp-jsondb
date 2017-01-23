/*
 * Controller.cpp
 *
 *  Created on: Dec 21, 2016
 *      Author: daniel
 */

#include "Session.h"

#include <sys/file.h>
#include <cstdio>
#include <utility>

#include "JsonException.h"
#include "Json.h"


namespace db {
namespace json {

void Session::setBasePath(const std::string& bp)  {
    _session._basePath = removeDoubleSlash(bp + "/");
    if (_session._basePath.empty()) _session._basePath = "jsondb/";
}
const std::string& Session::basePath() const {
    return _session._basePath;
}


class TransactionWriteFile
{
public:
    TransactionWriteFile(const std::string& filename, SessionImp& ctl);
    ~TransactionWriteFile();

    // Open file for writing. It also create a backup.
    void open(const std::string& filename);

    // Cancel write and restore element from backup.
    void rollbackAndClose();

    // Release lock and handle.
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

    void closeFile();
    void freeLock();

    std::string _filename;
    SessionImp& _ctl;
    FILE* _file;
};



TransactionWriteFile::TransactionWriteFile(const std::string& filename, SessionImp& ctl) :
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
    rename(filename.c_str(), (_filename+"~").c_str());
    checkForFileError("backup " + _filename);

    // Open a new file.
    _file = fopen(filename.c_str(), "w");
    checkForFileError("open " + _filename + " for writting");
    if (!_file) throw db::json::JsonException("Could not open " + _filename + " for writing : File don't exist.");

    // Prevent other process from editing the file.
    if (ENABLE_PROCESS_LOCK)
        if (flock(fileno(_file), LOCK_EX))
            throw db::json::JsonException("Could not lock " + _filename + " for writing.");

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
    if (_file) {
        // Enable other process from editing the file.
        if (ENABLE_PROCESS_LOCK)
            flock(fileno(_file), LOCK_UN);

        fclose(_file);
        _file = nullptr;
    }
}

void TransactionWriteFile::freeLock() {
    if (!_filename.empty()) {
        // Delete backup.
        remove((_filename+"~").c_str());

        _ctl.releaseLock(_filename);
        _filename.clear();
    }
}

void Session::doSave(JsonObject& root, const std::string& className) {
    TransactionWriteFile file(_session._basePath + className + root.name(), _session);
    root.asObject().writeFile(file.getFile());
}
void Session::doRemove(std::string filename) {
    // Use the TransactionFile to lock the file.
    TransactionWriteFile file(filename, _session);
    ::remove(filename.c_str());
}


} /* namespace json */
} /* namespace db */


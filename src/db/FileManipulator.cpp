/*
 * FileManipulator.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#include "FileManipulator.h"

#include <asm-generic/errno.h>
#include <asm-generic/errno-base.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>

#include "json/JsonException.h"

namespace db {

void checkForFileError(const std::string& filename) {
    switch (errno) {
        case ENOENT:
            break; // File don't exist. This is normal.
        case EFBIG:
            throw db::json::JsonException("Could not " + filename + " : File too big.");
        case ENAMETOOLONG:
            throw db::json::JsonException("Could not " + filename + " : Filename too long.");
        case EROFS:
        case EACCES:
            throw db::json::JsonException("Could not " + filename + " : missing permission.");
        case EMFILE:
            throw db::json::JsonException("Could not " + filename + " : too many file opened.");
        default:
            throw db::json::JsonException("Could not " + filename + " : Unknown reason.");
    }
}

std::vector<std::string> tokenize(std::string fullStr, std::string sep) {
    std::vector<std::string> token;

    int startPos = 0;
    int endPos = fullStr.find(sep, startPos);

    while (endPos != std::string::npos) {
        std::string part = fullStr.substr(startPos, endPos - startPos);
        token.push_back(part);

        startPos = endPos + 1;
        endPos = fullStr.find(sep, startPos);
    }

    if (startPos != fullStr.length()) {
        std::string part = fullStr.substr(startPos);
        token.push_back(part);
    } else {
        token.push_back("");
    }

    return token;
}

void createFolder(const std::string& folderPath) {
    auto tok = tokenize(folderPath, "/");

    // Clean the path, removing any down then up.
    std::string currentFolder;
    for (auto& ite : tok) {
        // Skip  "//" and "..".
        if (ite == "" || ite.empty() || ite == "..") continue;

        currentFolder += (currentFolder.empty() ? "" : "/") + ite;

        if (!mkdir(currentFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            switch (errno) {
                case EEXIST:
                case 0:
                    break; // Sucess, or already exist.
                case EACCES:
                case EROFS:
                    throw db::json::JsonException(
                            "Could not create folder " + currentFolder + ": we don't have permission.");
                case ELOOP:
                case EMLINK:
                    throw db::json::JsonException(
                            "Could not create folder " + currentFolder + ": Too many link (self looping?).");
                case ENAMETOOLONG:
                    throw db::json::JsonException(
                            "Could not create folder " + currentFolder + ": Name or path too long.");
                case ENOENT:
                    throw db::json::JsonException("Could not create folder " + currentFolder + ": Parent don't exist.");
                case ENOTDIR:
                    throw db::json::JsonException(
                            "Could not create folder " + currentFolder + ": Parent is not a directory.");
                case ENOSPC:
                    throw db::json::JsonException("Could not create folder " + currentFolder + ": Filesystem is full.");
                default:
                    throw db::json::JsonException("Could not create folder " + currentFolder + ": Unkown error " + std::to_string(errno) + " -> " + strerror(errno) + ".");
            };
        }
    }
}

std::vector<std::string> listJsonFile(const std::string& path) {
    std::vector<std::string> jsonFile;

    // List all json file in directory.
    ::DIR *dp;
    struct dirent *dirp;
    struct stat sb;

    int dfd = open(path.c_str(), O_RDONLY);
    if (dfd == -1) throw db::json::JsonException("Folder " + path + " don't exist");

    dp = opendir(path.c_str());
    if (dp == nullptr) throw db::json::JsonException("Folder " + path + " don't exist");

    while ((dirp = readdir(dp)) != nullptr) {
        std::string name(dirp->d_name);
        if (name.find_last_of(".json") != (name.length() - 5)) continue;

        jsonFile.push_back(path + dirp->d_name);
    }

    closedir(dp);

    return jsonFile;
}

std::string removeDoubleSlash(std::string txt) {
    if (!txt.empty()) {
        int pos = 0;
        while ((pos = txt.find("//", pos)) != std::string::npos)
            txt.replace(pos, 2, "/");
    }
    return txt;
}

//    void copyTo(FILE* src, FILE* dst) {
//        int size = 2048;
//        char buffer[2048];
//        do {
//            size = read(src, buffer, size);
//            write(dst, buffer, size);
//        } while(size == 2048);
//    }
//
//    bool isEmpty(FILE* target){
//        fseek(target, 0, SEEK_END);
//        bool emp = ftell(target) == 0;
//        fseek(target, 0, SEEK_SET);
//        return emp;
//    }

}// namespace db

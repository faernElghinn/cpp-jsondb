/*
 * FileManipulator.h
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#ifndef SRC_DB_FILEMANIPULATOR_H_
#define SRC_DB_FILEMANIPULATOR_H_

#include <string>
#include <vector>

namespace db {

// Split string on a token.
// FIXME : move to string helper function.
std::vector<std::string> tokenize(std::string txt, std::string sep);

// Create all parent directory.
void createFolder(const std::string& folderPath);

// Transform "//" into "/".
std::string removeDoubleSlash(std::string txt);


void checkForFileError(const std::string& filename);

// List all file to read. Will be 1 if id is set, all of class type if not.
std::vector<std::string> listJsonFile(const std::string& path);

}  // namespace db

#endif /* SRC_DB_FILEMANIPULATOR_H_ */

#pragma once

#include <string>

namespace elladan {
namespace jsondb {

class LockFile {
public:
   enum Type { EXCLUSIVE, SHARED };
   LockFile (const std::string& fname, Type type = EXCLUSIVE) ;
   ~LockFile();

private:
   int fd;
};

} } // namespace elladan::jsondb

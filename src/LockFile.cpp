#include "LockFile.h"

#include <elladan/Exception.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <filesystem>

namespace elladan {
namespace jsondb {

LockFile::LockFile (const std::string& fname, Type type) {
   fd = open(fname.c_str(), O_WRONLY);
   if (fd == -1)
      throw Exception(std::string("Could not open lockFile ") +  fname);

   flock(fd, type == EXCLUSIVE ? LOCK_EX : LOCK_SH);
}

LockFile::~LockFile() {
   if (fd != -1) {
      flock(fd, LOCK_UN);
      close(fd);
   }
}

} } // namespace elladan::jsondb

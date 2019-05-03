#include "JsonDb.h"

#include <fstream>
#include <filesystem>

#include "LockFile.h"
#include "Query.h"

namespace elladan {
namespace jsondb {


std::optional<json::Json> JsonDb::load(const std::string& className, const json::Json& id) const {
   return find(className).id(id).load();
}

Query JsonDb::find(const std::string& className) const {
   return Query(className, *this);
}

void JsonDb::save(const std::string& className, const json::Json& id, const json::Json& json) const {
   const std::string target = config.basePath + '/' + className;
   const std::string tmp = target + ".swap";
   LockFile (target + ".lock");

   json::Json allElement;

   std::ifstream file(target);
   if (file.is_open()) {
      allElement = config.read(file);
      file.close();
   }

   allElement.as<json::Object>().set(std::to_string(id), json);

   std::ofstream out(tmp);
   config.write(allElement, out);
   out.close();

   std::filesystem::rename(std::filesystem::path(tmp), std::filesystem::path(target));
   // FIXME: version check?
   // FIXME: secure multiThread and multiProcess.
}




} } // namespace elladan::jsondb

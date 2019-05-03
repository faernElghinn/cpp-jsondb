#pragma once

#include <elladan/json/json.h>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace elladan {
namespace jsondb {

class Query;
constexpr static char ID[] = "__id";


//
//class Fstream : public std::fstream{
//public:
//   Fstream (const std::string& fname )
//};
//
//int main()
//{
//    int posix_handle = ::_fileno(::fopen("test.txt", "r"));
//
//    ifstream ifs(::_fdopen(posix_handle, "r"));
//
//    string line;
//    getline(ifs, line);
//    ifs.close();
//    cout << "line: " << line << endl;
//    return 0;
//}
//bool OpenFileForSequentialInput(ifstream& ifs, const std::string& fname)
//{
//    ifs.open(fname.c_str(), ios::in);
//    if (! ifs.is_open()) {
//        return false;
//    }
//
//    using FilebufType = __gnu_cxx::stdio_filebuf<std::ifstream::char_type>;
//    static_assert(  std::is_base_of<ifstream::__filebuf_type, FilebufType>::value &&
//                    (sizeof(FilebufType) == sizeof(ifstream::__filebuf_type)),
//            "The filebuf type appears to have extra data members, the cast might be unsafe");
//
//    const int fd = static_cast<FilebufType*>(ifs.rdbuf())->fd();
//    assert(fd >= 0);
//    if (0 != posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL)) {
//        ifs.close();
//        return false;
//    }
//
//    return true;
//}

//template <typename T>
//size_t hash(){
//   return typeid(T).hash_code();
//}

struct JsonDbConfig {
   // Set the DB base folder. Default to "./data".
   std::string basePath;

   /*
    * Function used to write the data.
    * Overwrite to configure the Decoding option.
    * By default, use the more efficient bson format, allowing null value.
    */
   std::function<void(const json::Json& data, std::ostream& out)> write;

   /*
    * Function used to read the data.
    * Overwrite to configure the Decoding option.
    * By default, use the more efficient bson format, allowing null value.
    */
   std::function<json::Json(std::istream& in)> read;

   /*
    * Function used to extract the data.
    * Overwrite to configure the Decoding option.
    * By default, use the more efficient bson format, allowing null value.
    */
   std::function<std::vector<json::Json>(std::istream& in, const std::string& path)> extract;
};

struct JsonDb{
   JsonDbConfig config;

   std::optional<json::Json> load(const std::string& className, const json::Json& id) const ;
   Query find(const std::string& className) const;
   void save(const std::string& className, const json::Json& id, const json::Json& json) const;


protected:
   friend struct Query;
};



} } // namespace elladan::jsondb

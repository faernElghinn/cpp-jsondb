# cpp-jsondb
Local Json Db, akin of SqlLite for mongodb. 

Support recursive serialisation of to Json/bson file.
You can search be Object type (collection) and ObjectId.
Search/filtering support is very limited and should be done on the user side.

**Require cpp-json and cpp-library to work**


## Recursive (De)Serilization Example

In order to (de)serialize, your custom class must implement the public template persist(A& a), which map the class/struct fields to their json counterpart.
The library support nested value, as long as each nested value either posess the persist() template, is a std::set, a std::array, a std::map, or a primary data type. 

### Recursive (De)Serilization Example
```
#include <JsonDb/wrapper/Session.h>
#include <map>
#include <string>
using elladan::jsondb;
using elladan::jsondb;

class Serilizer {
public:
  template <typename A>
  void persist(A& a) {
      // The id must be unique, otherwise it will overwrite the previous value with the same id. 
      // The autoGenerate argument is not supported yet.
      id(a, _anUniqueNumber); 
      
      // field() map the member element to the json name.
      field(a, _aString, "theJsonName");
      
      // Commun c++ data structure are supported.
      field(a, _mapOfStringWithValue, "willBeAJsonMapOfStringWithValue");
  }
  
private:
  int _id = 0;
  std::string _aString;
  std::map<string, bool> _mapOfStringWithValue;
}
```

#include "./ObjectId.h"
#include <cstring>
#include <chrono>

#include <elladan/Random.h>
#include <elladan/Exception.h>


using namespace std::chrono;

namespace elladan {
namespace jsondb {

std::atomic<int> ObjectId::serial = Random::get(1 << 12);

ObjectId::ObjectId() {
   memset(data, 0, size);
}
ObjectId::ObjectId(const std::string& str) {
   if (str.size() != size*2)
     throw Exception("Invalid UUID hex string");

   data[3] = (uint8_t) stoi(str.substr( 0, 2), 0, 16);
   data[2] = (uint8_t) stoi(str.substr( 2, 2), 0, 16);
   data[1] = (uint8_t) stoi(str.substr( 4, 2), 0, 16);
   data[0] = (uint8_t) stoi(str.substr( 6, 2), 0, 16);

   data[8] = (uint8_t) stoi(str.substr( 8, 2), 0, 16);
   data[7] = (uint8_t) stoi(str.substr(10, 2), 0, 16);
   data[6] = (uint8_t) stoi(str.substr(12, 2), 0, 16);
   data[5] = (uint8_t) stoi(str.substr(14, 2), 0, 16);
   data[4] = (uint8_t) stoi(str.substr(16, 2), 0, 16);

   data[11] = (uint8_t) stoi(str.substr(18, 2), 0, 16);
   data[10] = (uint8_t) stoi(str.substr(20, 2), 0, 16);
   data[9]  = (uint8_t) stoi(str.substr(22, 2), 0, 16);
}

ObjectId ObjectId::generate(){
   ObjectId id;

   *((uint32_t*)(id.data+0)) = duration_cast<seconds>(system_clock::now().time_since_epoch()).count(); // FIXME : use c++20 UTC time.
   *((uint32_t*)(id.data+4)) = Random::get(UINT32_MAX + 1); 
   *((uint8_t *)(id.data+8)) = Random::get(UINT8_MAX + 1);
   int ser = ++serial;
   *((uint8_t *)(id.data+9))  = ser & 0xff;
   *((uint16_t*)(id.data+10)) = (ser >> 4) & 0xffff;

   return id;
}

std::string ObjectId::toString() const{
   char s[size * 2 + 1];
   snprintf(s, 25, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
      data[3], data[2], data[1], data[0],
      data[8], data[7], data[6], data[5], data[4],
      data[11], data[10], data[9]
   );
   return s;
}

time_t ObjectId::epoch() const{
   return *((int32_t*)data+0);
}


bool ObjectId::operator < (const ObjectId& rhs) const { return memcmp(data, rhs.data, size) <  0;}
bool ObjectId::operator <=(const ObjectId& rhs) const { return memcmp(data, rhs.data, size) <= 0;}
bool ObjectId::operator ==(const ObjectId& rhs) const { return memcmp(data, rhs.data, size) == 0;}
bool ObjectId::operator !=(const ObjectId& rhs) const { return memcmp(data, rhs.data, size) != 0;}
bool ObjectId::operator >=(const ObjectId& rhs) const { return memcmp(data, rhs.data, size) >= 0;}
bool ObjectId::operator > (const ObjectId& rhs) const { return memcmp(data, rhs.data, size) >  0;}

}}

namespace std {
std::string to_string(const elladan::jsondb::ObjectId& oid){
   return oid.toString();
}
}
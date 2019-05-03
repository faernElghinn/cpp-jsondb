#pragma once

#include <string>
#include <cstdint>
#include <atomic>

namespace elladan {
namespace jsondb {

struct ObjectId {
  static constexpr size_t size = 12;

  ObjectId();
  ObjectId(const std::string& hexString);

  static ObjectId generate();

  std::string toString() const;
    
  time_t epoch() const;

  uint8_t data[size];

  bool operator < (const ObjectId& rhs) const;
  bool operator <=(const ObjectId& rhs) const;
  bool operator ==(const ObjectId& rhs) const;
  bool operator !=(const ObjectId& rhs) const;
  bool operator >=(const ObjectId& rhs) const;
  bool operator > (const ObjectId& rhs) const;
 
 protected:
   static std::atomic<int> serial;
};

}}

namespace std {
   std::string to_string(const elladan::jsondb::ObjectId& );
}

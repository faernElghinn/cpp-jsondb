/*
 * Action.h
 *
 *  Created on: Dec 19, 2016
 *      Author: daniel
 */

#ifndef SRC_ACTION_H_
#define SRC_ACTION_H_

#include <elladan/Exception.h>
#include <elladan/json/json.h>
#include <elladan/Stringify.h>
#include <elladan/UUID.h>
#include <elladan/VMap.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <set>

namespace elladan {
namespace jsondb {

class IdIndex;

template <typename T>
struct has_persist
{
   struct dummy {
      void persist() {}
   };

   template <typename C, typename P>
   static auto test(P * p) -> decltype(std::declval<C>().persist(*p), std::true_type());

   template <typename, typename>
   static std::false_type test(...);

   typedef decltype(test<T, dummy>(nullptr)) type;
   static const bool value = std::is_same<std::true_type, decltype(test<T, dummy>(nullptr))>::value;
};

template <typename T>
struct is_map
{
   template <typename U>
   static typename std::enable_if<(
         std::is_same<U, std::map< typename U::key_type, typename U::mapped_type, typename U::key_compare, typename U::allocator_type>
   >::value), char>::type
   test(typename U::value_type* x);

   template <typename U>
   static long test(U* x);

   static const bool value = sizeof(test<T>(nullptr)) == 1;
};

template <typename T>
struct is_vmap
{
   template <typename U>
   static typename std::enable_if<(
         std::is_same<U, elladan::VMap< typename U::key_type, typename U::value_type>
   >::value), char>::type
   test(typename U::value_type* x);

   template <typename U>
   static long test(U* x);

   static const bool value = sizeof(test<T>(nullptr)) == 1;
};

template <typename T>
struct is_set
{
   template <typename U>
   static typename std::enable_if<(
         std::is_same<U, std::set< typename U::key_type, typename U::key_compare, typename U::allocator_type>
   >::value), char>::type
   test(typename U::value_type* x);

   template <typename U>
   static long test(U* x);

   static const bool value = sizeof(test<T>(0)) == 1;
};

template <typename T>
struct is_vector
{
   template <typename U>
   static typename std::enable_if<(
         std::is_same<U, std::vector< typename U::value_type, typename U::allocator_type >
   >::value), char>::type
   test(typename U::value_type* x);

   template <typename U>
   static long test(U* x);

   static const bool value = sizeof(test<T>(0)) == 1;
};




// Parse the json and put it in the value.
class ReadAction
{
public:
   ReadAction(json::JsonObject_t obj = std::make_shared<json::JsonObject>()) : doc(obj) { }

   template<typename T>
   typename std::enable_if_t<has_persist<T>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t obj = getCastValue<json::JsonObject>("persist", name);

      ReadAction subAction(obj);
      subAction.path += path + name + "/";
      ele.persist(subAction);
   }

   // Maps
   template<typename T>
   typename std::enable_if_t<is_map<T>::value && has_persist<typename T::mapped_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t map = getCastValue<json::JsonObject>("persist map", name);

      for (auto& ite : map->value) {
         ReadAction subAction(std::dynamic_pointer_cast<json::JsonObject>(ite.second));
         subAction.path += path + name + "/";
         ele[to_string(ite.first)].persist(subAction);
      }
   }
   template<typename T>
   typename std::enable_if_t<is_map<T>::value && !has_persist<typename T::mapped_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t map = getCastValue<json::JsonObject>("map", name);

      for (auto& ite2 : map->value) {
         typename T::key_type key = fromString< typename T::key_type > (ite2.first);
         ele[key] = elladan::json::fromJson<typename T::mapped_type>(ite2.second);
      }
   }

   // VMaps
   template<typename T>
   typename std::enable_if_t<is_vmap<T>::value && has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t map = getCastValue<json::JsonObject>("persist vmap", name);

      for (auto& ite : map->value) {
         ReadAction subAction(std::dynamic_pointer_cast<json::JsonObject>(ite.second));
         subAction.path += path + name + "/";
         ele[to_string(ite.first)].persist(subAction);
      }
   }
   template<typename T>
   typename std::enable_if_t<is_vmap<T>::value && !has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t map = getCastValue<json::JsonObject>("vmap", name);

      for (auto& ite2 : map->value) {
         typename T::key_type key = fromString<typename T::key_type >(ite2.first);
         ele[key] = elladan::json::fromJson<typename T::value_type>(ite2.second);
      }
   }

   // Vector
   template<typename T>
   typename std::enable_if_t<is_vector<T>::value && has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonArray_t list = getCastValue<json::JsonArray>("persist vector", name);

      for (auto& ite : list->value) {
         if (ite->getType() != elladan::json::JSON_OBJECT)
            throw Exception("sub element of " + name + " should be objects");

         typename T::value_type element;
         ReadAction subAction(std::dynamic_pointer_cast<json::JsonObject>(ite));
         subAction.path += path + name + "/";
         element.persist(subAction);

         ele.push_back(element);
      }
   }
   template<typename T>
   typename std::enable_if_t<is_vector<T>::value && !has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonArray_t list = getCastValue<json::JsonArray>("vector", name);
      ele.clear();
      for (auto& ite : list->value)
         ele.push_back(elladan::json::fromJson<typename T::value_type>(ite));
   }

   // Set
   template<typename T>
   typename std::enable_if_t<is_set<T>::value && has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonArray_t list = getCastValue<json::JsonArray>("persist set", name);
      for (auto& ite : list->value) {
         if (ite->getType() != elladan::json::JSON_OBJECT)
            throw Exception("sub element of " + name + " should be objects");

         typename T::value_type element;
         ReadAction subAction(std::dynamic_pointer_cast<json::JsonObject>(ite));
         subAction.path += path + name + "/";
         element.persist(subAction);
         ele.insert(element);
      }
   }
   template<typename T>
   typename std::enable_if_t<is_set<T>::value && !has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonArray_t list = getCastValue<json::JsonArray>("set", name);

      ele.clear();
      for (auto& ite : list->value)
         ele.insert(elladan::json::fromJson<typename T::value_type>(ite));
   }

   // enum
   template<typename T>
   typename std::enable_if_t<std::is_enum<T>::value>
   doAction(T& ele, const std::string& name) {
      typename std::underlying_type<T>::type val;
      doAction(val, name);
      ele = (T) val;
   }

   template<typename T>
   typename std::enable_if_t<!(is_map<T>::value || is_vmap<T>::value || has_persist<T>::value || is_set<T>::value || is_vector<T>::value || std::is_enum<T>::value)>
   doAction(T& ele, const std::string& name) {
      validateDoc("default");
      ele = elladan::json::fromJson<T>(getValue(name, "default"));
   }

   template<typename Element>
   void doActionId(Element& ele, bool autoGen) {
      throw Exception(std::string("Invalid read - ID only support type supported by to_string() "));
   }

   json::JsonObject_t doc;
   std::string path;

private:
   void validateDoc(const std::string& error);
   json::Json_t getValue(const std::string& name, const std::string& error);

   template <typename T>
   std::shared_ptr<T> castValue(json::Json_t val, const std::string& name, const std::string& error) {
      if (val->getType() != T::TYPE)
         throw Exception("Invalid " + error +" read - " + name + " is not an object.");
      return std::static_pointer_cast<T>(val);
   }

   template <typename T>
   std::shared_ptr<T> getCastValue(const std::string& error, const std::string& name){
      validateDoc(error);
      auto val = getValue(name, error);
      return castValue<T>(val, name, error);
   }

};
// Id
template<> void ReadAction::doActionId<int>(int& ele, bool autoGen);
template<> void ReadAction::doActionId<std::string>(std::string& ele, bool autoGen);
template<> void ReadAction::doActionId<UUID>(UUID& ele, bool autoGen);


// Convert to object from the object to json.
class WriteAction
{
public:
   WriteAction(json::Json_t obj = json::Json_t(), std::shared_ptr<IdIndex> idx = std::shared_ptr<IdIndex>()) : doc(obj), _idx(idx) {}

   template<typename T>
   typename std::enable_if_t<has_persist<T>::value>
   doAction(T& ele, const std::string& name) {
      WriteAction subAction(std::make_shared<json::JsonObject>());
      subAction.path += path + name + "/";
      ele.persist(subAction);
      asDoc("persist")->value[name] = subAction.doc;
   }

   // Maps of value with persist
   template<typename T>
   typename std::enable_if_t<is_map<T>::value && has_persist<typename T::mapped_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t obj = std::make_shared<json::JsonObject>();
      asDoc("persist map")->value[name] = obj;

      for (auto& ite : ele) {
         WriteAction subAction(std::make_shared<json::JsonObject>());
         subAction.path += path + name + "/";
         ite.second.persist(subAction);
         obj->value[to_string(ite.first)] = subAction.doc;
      }

   }

   // Maps of value without persist
   template<typename T>
   typename std::enable_if_t<is_map<T>::value && !has_persist<typename T::mapped_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t obj = std::make_shared<json::JsonObject>();
      asDoc("map")->value[name] = obj;

      for (auto& ite : ele)
         obj->value[to_string(ite.first)] = json::toJson(ite.second);
   }

   // VMaps of value with persist
   template<typename T>
   typename std::enable_if_t<is_vmap<T>::value && has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t obj = std::make_shared<json::JsonObject>();
      asDoc("persist map")->value[name] = obj;

      for (auto& ite : ele) {
         WriteAction subAction(std::make_shared<json::JsonObject>());
         subAction.path += path + name + "/";
         ite.second.persist(subAction);
         obj->value[to_string(ite.first)] = subAction.doc;
      }

   }

   // VMaps of value without persist
   template<typename T>
   typename std::enable_if_t<is_vmap<T>::value && !has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonObject_t obj = std::make_shared<json::JsonObject>();
      asDoc("map")->value[name] = obj;

      for (auto& ite : ele)
         obj->value[to_string(ite.first)] = json::toJson(ite.second);
   }

   // Vector of value with persist
   template<typename T>
   typename std::enable_if_t<(is_vector<T>::value) && has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonArray_t array = std::make_shared<json::JsonArray>();
      asDoc("persist array")->value[name] = array;

      for (auto& ite : ele) {
         WriteAction subAction(std::make_shared<json::JsonObject>());
         subAction.path += path + name + "/";
         ite.persist(subAction);
         array->value.push_back(subAction.doc);
      }
   }

   // Vector of value without persist
   template<typename T>
   typename std::enable_if_t<(is_vector<T>::value) && !has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonArray_t obj = std::make_shared<json::JsonArray>();
      asDoc("array")->value[name] = obj ;
      for (auto ite : ele)
         obj->value.push_back(json::toJson(ite));
   }

   // Set of value with persist
   template<typename T>
   typename std::enable_if_t<(is_set<T>::value) && has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonArray_t array = std::make_shared<json::JsonArray>();
      asDoc("persist array")->value[name] = array;

      for (auto ite : ele) {
         WriteAction subAction(std::make_shared<json::JsonObject>());
         subAction.path += path + name + "/";
         ite.persist(subAction);
         array->value.push_back(subAction.doc);
      }
   }

   // Set of value without persist
   template<typename T>
   typename std::enable_if_t<(is_set<T>::value) && !has_persist<typename T::value_type>::value>
   doAction(T& ele, const std::string& name) {
      json::JsonArray_t obj = std::make_shared<json::JsonArray>();
      asDoc("array")->value[name] = obj ;
      for (auto ite : ele)
         obj->value.push_back(json::toJson(ite));
   }

   // enum
   template<typename T>
   typename std::enable_if_t<std::is_enum<T>::value>
   doAction(T& ele, const std::string& name) {
      typename std::underlying_type<T>::type val;
      doAction(val, name);
   }

   template<typename T>
   typename std::enable_if_t<!(is_map<T>::value || is_vmap<T>::value || has_persist<T>::value || is_set<T>::value || is_vector<T>::value || std::is_enum<T>::value)>
   doAction(T& ele, const std::string& name) {
      asDoc("default")->value[name] = json::toJson(ele);
   }

   template<typename T>
   void doActionId(T& ele, bool autoGen) {
      throw Exception(std::string("Invalid read - ID only support type supported by to_string() "));
   }

   std::shared_ptr<IdIndex> _idx;
   json::Json_t doc;
   std::string path;

private:
   inline json::JsonObject_t asDoc(const std::string& error) {
      if (!doc || doc->getType() != json::JSON_OBJECT)
         throw Exception("Invalid " + error + " write - " + path + " is not an object");
      return std::dynamic_pointer_cast<json::JsonObject>(doc);
   }
};

template<> void WriteAction::doActionId<int>(int& ele, bool autoGen);
template<> void WriteAction::doActionId<std::string>(std::string& ele, bool autoGen);
template<> void WriteAction::doActionId<UUID>(UUID& ele, bool autoGen);

// Delete action map the id and ignore everything else.
class DeleteAction
{
public:
   DeleteAction() : doc(nullptr) {}

   template<typename Element>
   void doAction(Element& ele, const std::string& name) {    }

   template<typename Element>
   void doActionId(Element& ele, bool autoGen) {
      throw Exception(std::string("Invalid delete - ID only support type supported by to_string "));
   }

   json::JsonObject_t doc;
   std::string path;

private:
   json::JsonObject_t validateDoc(const std::string& error) ;
};
// Id
template<> void DeleteAction::doActionId<int>(int& ele, bool autoGen);
template<> void DeleteAction::doActionId<std::string>(std::string& ele, bool autoGen);
template<> void DeleteAction::doActionId<UUID>(UUID& ele, bool autoGen);
// FIXME: all doActionId should be updated to use "to_string/fromString"






template<typename Action, typename Element>
void field(Action& a, Element& ele, const std::string& name) {
   a.doAction(ele, name);
}
template<typename Action, typename Element>
void id(Action& a, Element& ele, bool autoGen = true) {
   a.doActionId(ele, autoGen);
}



} } // namespace elladan::jsondb

#endif /* ACTION_H_ */

/*
 * JsonDb.h
 *
 *  Created on: Aug 30, 2016
 *      Author: daniel
 */

#ifndef SRC_JSONDB_H_
#define SRC_JSONDB_H_


/* Logic, organization and everything but code.
 *
 * The goal : offer an db like implementation of database with json files as a backend.
 *
 * Interface :
 *  - set a directory as the data root.
 *  - Give an API to make class known to this library
 *    - This should map a subfolder name to each class.
 *    - This should allow a simple load / write on the class
 *  - The lib will manage caching, concurrent access, parsing (partial), saving. loading and deleting of data
 *    - Caching should be hintable by class.
 *  - The library should give easy search and sort function
 *  - TODO: what about related table (SetNullOnDelete, CascadeDelete)?
 *  - TODO: what about indexing?
 *  - TODO: how about version updating?
 *
 *  Implementation
 *
 *  - Api:
 *  The API should use something alike Wt::Dbo -> it simply request a template function to be in the class.
 *
 *  So, the way it works:
 *
 *
 *  template<typename A> void persist(A& action) { ... to be filled based on class ... }
 *  This is the binding function. The action is a function created y this library.
 *  - Possible action are : load, save, delete, (search?)
 *  In it, there will be binding to link the class member with the db:
 *  - json_map_index(action, member_element, "NameInJson", AutoGenerateFlag | ManualGenerationFlag);
 *     Map an index field. Can be autoGenerated on create.
 *     The lib will keep a map of index for quick reference.
 *     All item MUST have an index, it will be used as the file name.
 *     No 2 index can be the same. (duh!)
 *  - json_map_version(action, (int)member_element, "NameInJson");
 *     Map an version field. Version will be auto incremented. Must be an int.
 *     The lib can warn when the item was updated using this field.
 *  - json map_value(action, member_element, "NameInJson");
 *     Map an member variable to an element. The type must be a basic element.
 *  - json map_complex<type>(action, member_element, "NameInJson");
 *     Map complex object. This recursively call persist on the sub object, creating a subsection for it. Sub-object don't need an id.
 *  - json map_section<vector|map>(action, "NameInJson");
 *     Map section. The section can be used as the action for sub-element.
 *  - json map_reference<vector|map>(action, member_element, "NameInJson");
 *     Map a reference, will link the index of the other value.
 *
 *  Actions:
 *  - Save : Use the value binding to create the Json txt.
 *  - Load : Use the value binding to read the Json txt.
 *
 */


////
////class AnotherJson{
////};
////class LinkedJson{
////};
////class TestMeJson{
////    UUID id; // Can be overwritten! Will be used as filename.
////    int version; // Can be overwritten or absent!
////
////    bool asBool;
////    int asInt;
////    long asLong;
////    float asFloat;
////    double asDouble;
////    char asChar;
////    std::string asString;
////    //Wt::WString asWString; // Simply add a to/fromUtf8();
////
////    AnotherJson includedElement;
////    std::vector<AnotherJson> elementList;
////    std::set<AnotherJson> elementSet; // Internally managed as a vector
////    std::map<ToStringAbleElement, Any> elementMap;
////
////
////    UUID somethingElse;
////
////    template<typename A>
////    void persist(A action){
////        mapClass<TestMeJson>("test_table"); // Once for class.
////
////        Wt::Dbo::Id(a, authName);
////        Wt::Dbo::Version(a, authName);
////        Wt::Dbo::NoVersion(a);
////
////        Wt::Dbo::field(a, asBool,             "asBool");
////        Wt::Dbo::field(a, asInt,              "asInt");
////        Wt::Dbo::field(a, asLong,             "asLong");
////        Wt::Dbo::field(a, asFloat,            "asFloat");
////        Wt::Dbo::field(a, asDouble,           "asDouble");
////        Wt::Dbo::field(a, asChar,             "asChar");
////        Wt::Dbo::field(a, includedElement,    "includedElement");
////        Wt::Dbo::field(a, elementList,        "elementList");
////
////        Wt::Dbo::linkedTo(a, somethingElse, "linked_json_db_name/path/to/value", OnEditDelete|OnDeleteSetNull|OnDeleteCascade);
////    }
////};
////
////
////




// flockfile(FILE * fd) : synchronize file access between THREADS : will lock until file is freed.
// funlockfile(FILE * fd) : unlock file
// r = flock(fileno(fp), LOCK_EX); : synchronize file access between process : will lock until file is freed.
// r = flock(fileno(fp), LOCK_UN); : synchronize file access between process : free lcok.

#endif /* SRC_JSONDB_H_ */

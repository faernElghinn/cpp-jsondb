/*
 * Clause.h
 *
 *  Created on: May 23, 2017
 *      Author: daniel
 */

#pragma once

#include <elladan/json/json.h>
#include <set>
#include <string>
#include <vector>

namespace elladan {
namespace jsondb {

class JsonDb;
typedef std::set<json::Json_t, json::cmpJson> JsonTSet;


class Clause
{
protected:
    Clause(){}
    virtual ~Clause(){}
public:
    static std::shared_ptr<Clause> And();
    static std::shared_ptr<Clause> Or();
    static std::shared_ptr<Clause> Exist(const std::string& path);
    static std::shared_ptr<Clause> CmpLT(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
    static std::shared_ptr<Clause> CmpLE(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
    static std::shared_ptr<Clause> CmpEQ(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
    static std::shared_ptr<Clause> CmpNE(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
    static std::shared_ptr<Clause> CmpGE(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);
    static std::shared_ptr<Clause> CmpGT(const std::string& path, elladan::json::Json_t value, bool caseSensitive = true);

    // Return true if the element match the filter condition.
    virtual bool filter(json::JsonObject_t root) const = 0;
    virtual void filter(JsonTSet& resultSet, const std::string& clas, JsonDb& db) const = 0;
};

} } // namespace elladan::jsondb

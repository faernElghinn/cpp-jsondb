/*
 * JsonDb.h
 *
 *  Created on: May 18, 2017
 *      Author: daniel
 */

#pragma once

#include <elladan/json/json.h>
#include <elladan/VMap.h>
#include <condition_variable>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "Index.h"
#include "Sort.h"

namespace elladan {
namespace jsondb {

#if 0
#define DEBUG(MSG, ...) printf("[debug] %s : " MSG "\n", __PRETTY_FUNCTION__, __VA_ARGS__)
#else
#define DEBUG(MSG, ...) do {} while (0)
#endif

constexpr static char ID[] = "id";
constexpr static char ID_PATH[] = "/id";

class Clause;

class JsonDb
{
public:
    JsonDb();
    ~JsonDb(){}

    // Set the DB base folder.
    void setBasePath(const std::string& basePath);

    // Set the decoding option. Only required if you imported data manually.
    void setDecodingOption(json::DecodingOption decodingFlags);

    void setFormat(json::StreamFormat format);

    // FIXME : mark idx as not yet available, or cache idx and add an update function.
    void addIndex(const std::string& clas, const std::string& path, bool unique = false, bool caseSensitive = false);
    void setId(const std::string& clas, json::Json_t defaultValue, bool caseSensitive = false);

    // Flush all index and reload all of them.
    void scanIndex();

    void save(const std::string& clas, json::Json_t id, json::Json_t json);
    size_t count(const std::string& clas);
    size_t countConditionnal(const std::string& clas, const jsondb::Clause& where);
    json::Json_t load(const std::string& clas, json::Json_t id);
    std::vector<json::Json_t> loadAll(std::string clas, Sort = Sort());
    std::vector<json::Json_t> loadConditionnal(const std::string& clas, const Clause& where, Sort = Sort());
    void remove(const std::string& clas, json::Json_t id);
    void remove(const std::string& clas, const jsondb::Clause& where);

    inline const std::string& getBasePath() const { return _basepath;}
    inline const elladan::VMap<std::string, elladan::VMap<std::string, std::shared_ptr<Index>>>& getIndex() const { return _indexes;}

    // Get only part of the document.
    std::vector<json::Json_t> extract(const std::string& clas, json::Json_t id, const std::string& path);

    // Lock a file. Good only for intra-process locking.
    void getLock(const std::string& filename);

    // Release a previously locked file.
    void releaseLock(const std::string& filename);

    std::shared_ptr<IdIndex> getIdGen(const std::string& clas);
    json::Json_t genId(const std::string& clas);

protected:
    std::set<std::string> _fileLock;
    std::mutex _fileMutex;
    std::condition_variable _fileCond;

    std::string _basepath;
    json::StreamFormat _format;
    json::DecodingOption _decodingOption;

    // A map of class and its indexed path.
    // Used to index queries.
    elladan::VMap<std::string, elladan::VMap<std::string, std::shared_ptr<Index>>> _indexes;

};

} } // namespace elladan::jsondb

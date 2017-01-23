/*
 * Statement.h
 *
 *  Created on: Jan 5, 2017
 *      Author: daniel
 */

#ifndef SRC_DB_JSON_MANAGER_READSTATEMENT_H_
#define SRC_DB_JSON_MANAGER_READSTATEMENT_H_

#include <string>
#include <vector>

#include "../FileManipulator.h"
#include "../UUID.h"
#include "Action.h"
#include "Json.h"
#include "JsonException.h"
#include "Session_impl.h"
#include "TransactionReadFile.h"
#include "../Convertor.h"

namespace db {
namespace json {

constexpr bool ENABLE_PROCESS_LOCK = 0;

class Clause
{
public:
    Clause() {
    }
    virtual ~Clause() {
    }
    virtual bool filter(JsonObject& root) = 0;
};

class AndClause: public Clause
{
public:
    std::vector<Clause*> _clauses;
    AndClause() {
    }
    bool filter(JsonObject& root) {
        for (auto ite : _clauses)
            if (!ite->filter(root)) return false;
        return true;
    }
    AndClause* add(Clause* c) {
        _clauses.push_back(c);
        return this;
    }
};

class OrClause: public Clause
{
public:
    std::vector<Clause*> _clauses;
    OrClause() {
    }
    bool filter(JsonObject& root) {
        for (auto ite : _clauses)
            if (ite->filter(root)) return true;
        return false;
    }
    OrClause* add(Clause* c) {
        _clauses.push_back(c);
        return this;
    }
};

template<typename T>
class CmpClause: public Clause
{
public:
    enum Mode
    {
        NE, LT, LE, EQ, GE, GT
    };

    T _expected;
    std::string _path;
    Mode _mode;
    CmpClause(const std::string& path, Mode mode, T value) :
            _path(path), _expected(value), _mode(mode) {
    }
    bool filter(JsonObject& root) {

        for (auto ite : root.findValues(_path)) {
            T value;
            if (!getValue(ite, value))
                throw db::json::JsonException("Element " + _path + " of " + root.name() + " is of the wrong type.");

            switch (_mode) {
                case NE:
                    if (value == _expected)
                        return false;
                    else break;
                case EQ:
                    if (value == _expected)
                        return true;
                    else break;
                default:
                    if (testRelation(value))
                        return true;
                    else break;
            }

        }

        return _mode == NE;
    }

    bool testRelation(T value) {
        switch (_mode) {
            case LT:
                return value < _expected;
            case LE:
                return value <= _expected;
            case GE:
                return value >= _expected;
            case GT:
                return value > _expected;
            default:
                return false;
        }
    }
};
template<> bool CmpClause<std::string>::testRelation(std::string value);

template<typename T>
bool getValue(JsonElement& ele, T& val);

#define GetValueType(Type, Name) template<> bool getValue<Type>(JsonElement& ele, Type& val);
GetValueType(bool, Bool)
GetValueType(int, Int)
GetValueType(long, Int)
GetValueType(unsigned, Int)
GetValueType(float, Float)
GetValueType(double, Float)
GetValueType(std::string, String)
#undef GetValueType

/// Manage read only query.
template<typename C>
class ReadStatement
{
public:
    ReadStatement(const std::string& classPath, SessionImp& ctl) :
            _ctl(ctl), _classPath(classPath), _clause(nullptr) {
    }

    // Do the actual read and parsing.
    std::vector<C> findAll() {
        // Load and parse all files.
        std::vector<C> result;
        for (auto& ite : findFileToParse(_classPath)) {
            TransactionReadFile src(ite, _ctl);
            if (!src) continue;

            JsonObject root = JsonObject::readFile(src.getFile());

            C ele;
            db::json::ReadAction action(root);
            ele.persist(action);
            result.emplace_back(ele);
        }
        return result;
    }
    C findOne() {
        auto all = findAll();
        if (all.empty()) return C();
        return all.front();
    }

    // Set the id to search for.
    template<typename T>
    ReadStatement<C>& id(const T& id) {
        _id = toString(id);
        return *this;
    }

    ReadStatement<C>& where(Clause *clause) {
        _clause = clause;
        return *this;
    }

private:
    // List all file to read. Will be 1 if id is set, all of class type if not.
    std::vector<std::string> findFileToParse(const std::string& folderName) {
        // For those with an id, simply fix the expected filename.
        std::vector<std::string> fileToParse;
        if (!_id.empty())
            fileToParse.push_back(folderName + _id);
        else fileToParse = listJsonFile(folderName);

        return fileToParse;
    }

private:
    std::string _id;
    std::string _classPath;
    SessionImp& _ctl;
    Clause * _clause;
};

} /* namespace json */
} /* namespace db */

#endif /* SRC_DB_JSON_MANAGER_READSTATEMENT_H_ */

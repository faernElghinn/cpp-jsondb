/*
 * Sort.h
 *
 *  Created on: May 26, 2017
 *      Author: daniel
 */

#pragma once

#include <elladan/json/json.h>
#include <string>
#include <utility>
#include <vector>

namespace elladan {
namespace jsondb {

class Sort
{
public:
    Sort();
    Sort(const std::string& path, bool asc = true);
    virtual ~Sort();

    void add(const std::string& path, bool asc = true);

    // Sort the elements according to the defined rules.
    void doSort(std::vector<json::Json_t>& results) const;

protected:
    typedef std::pair<std::string, bool> Sorter;
    std::vector<Sorter> _sorters;
};


} } // namespace elladan::jsondb

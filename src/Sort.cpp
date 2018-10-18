/*
 * Sort.cpp
 *
 *  Created on: May 26, 2017
 *      Author: daniel
 */

#include "Sort.h"

#include <algorithm>
#include <memory>

namespace elladan {
namespace jsondb {


Sort::Sort(){}
Sort::Sort(const std::string& path, bool asc){
    add(path, asc);
}
Sort::~Sort(){}

void Sort::add(const std::string& path, bool asc){
    _sorters.push_back(Sorter(path, asc));
}

// Sort the elements according to the defined rules.
void Sort::doSort(std::vector<json::Json_t>& results) const {
    if (_sorters.empty())
        return;

    struct CustomLess {
        const std::vector<Sorter>& sorters;
        CustomLess(const std::vector<Sorter>& s) : sorters(s) {}

        bool operator() (const json::Json_t a, const json::Json_t b) const {
            for (auto& ite : sorters) {
                auto left_list  = a->getChild(a, ite.first);
                auto right_list = b->getChild(b, ite.first);

                const json::Json* left = nullptr;
                if (!left_list.empty())
                    left = left_list.front().get();

                const json::Json* right = nullptr;
                if (!right_list.empty())
                    right = right_list.front().get();

                if (!left && !right) return ite.second;
                if (!left || !right) return !left ^ !ite.second;

                int i = left->cmp(right);
                if (i > 0) return !ite.second;
                if (i < 0) return ite.second;
            }
            return false;
        }
    };

    CustomLess customLess(_sorters);

    std::sort(results.begin(), results.end(), customLess);
}


} } // namespace elladan::jsondb

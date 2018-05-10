//
// Created by dogzik on 10.05.18.
//

#ifndef HW2_PATH_H
#define HW2_PATH_H

#include <vector>
#include <string>

char const SEP = '/';

struct path {
    path() = default;
    path add_to_path(std::string s) const;

    std::string to_string() const;

    bool operator<(path const& other) const;

    operator std::string() const;

    path resolve(path const& other) const;
private:
    std::vector<std::string> fragments;

    explicit path(std::vector<std::string> const& frags);
};


#endif //HW2_PATH_H

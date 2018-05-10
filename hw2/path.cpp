#include "path.h"
#include <memory>

using std::string;
using std::vector;
using std::move;

path::path(vector<string> const& frags): fragments(frags) {}

path path::add_to_path(string s) const {
    path res(fragments);
    res.fragments.emplace_back(move(s));
    return res;
}

string path::to_string() const {
    string res;
    for (size_t i = 0; i + 1 < fragments.size(); ++i) {
        res.append(fragments[i] + SEP);
    }
    if (!fragments.empty()) {
        res.append(fragments.back());
    }
    return res;
}

bool path::operator<(path const &other) const {
    if (fragments.size() == other.fragments.size()) {
        return to_string() < other.to_string();
    }
    return fragments.size() < other.fragments.size();
}

path::operator string() const {
    return to_string();
}

path path::resolve(path const &other) const {
    path res(*this);
    res.fragments.insert(res.fragments.end(), other.fragments.begin(), other.fragments.end());
}

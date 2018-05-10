#include "stream_finder.h"
#include <memory>

using std::string;
using std::move;

bool stream_finder::match() const noexcept {
    return matched;
}

void stream_finder::reset() noexcept {
    last = 0;
    matched = 0;
}

size_t stream_finder::iterate(char c) noexcept {
    size_t j = last;
    while (j > 0 && pattern[j] != c) {
        j = pf[j - 1];
    }
    j += (pattern[j] == c);
    return j;
}

stream_finder::stream_finder(string s)
        : pattern(move(s))
        , pf(pattern.size(), 0)
        , last(0)
        , matched(0) {
    for (size_t i = 1; i < pattern.size(); ++i) {
        pf[i] = last = iterate(pattern[i]);
    }
    last = 0;
}

stream_finder& stream_finder::operator<<(string const &s) {
    for (char c : s) {
        last = iterate(c);
        if (last == pf.size()) {
            matched = 1;
        }
    }
    return *this;
}

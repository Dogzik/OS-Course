#ifndef HW2_STREAM_FINDER_H
#define HW2_STREAM_FINDER_H

#include <cstdlib>
#include <vector>
#include <string>

struct stream_finder {
    explicit stream_finder(std::string s);

    stream_finder& operator<<(std::string const& s);

    bool match() const noexcept;

    void reset() noexcept;
private:
    std::string pattern;
    std::vector<size_t> pf;
    size_t last;
    bool matched;

    size_t iterate(char c) noexcept;
};


#endif //HW2_STREAM_FINDER_H

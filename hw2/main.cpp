#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "stream_finder.h"
#include "path.h"
#include "autocloseable.h"
#include "utils.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::runtime_error;
using std::set;
using std::pair;
using std::unordered_map;
using std::unordered_set;

struct my_stat {
    __mode_t mode;
    __blksize_t buff_size;
    __ino_t inod;

    my_stat() = default;

    explicit my_stat(struct stat data)
            : mode(data.st_mode), buff_size(data.st_blksize), inod(data.st_ino) {}
};

my_stat get_stat(string const &path) {
    struct stat tmp;
    if (stat(path.c_str(), &tmp) == -1) {
        throw runtime_error("Can't get stat for path: " + path);
    }
    return my_stat(tmp);
}

path get_cur_dir() {
    char *raw_dir = get_current_dir_name();
    if (raw_dir == nullptr) {
        throw runtime_error("Can't get current directory");
    }
    return path().add_to_path(raw_dir);
}

bool check_file(path const &f_path, stream_finder &finder, size_t buff_size) {
    try {
        auto file = autocloseable_file(f_path, O_RDONLY);
        finder.reset();
        char buff[buff_size];
        while (file.read_data(buff, buff_size)) {
            finder << buff;
            if (finder.match()) {
                return 1;
            }
        }
        return 0;
    } catch (runtime_error const &e) {
        print_error(e.what());
        return 0;
    }
}

vector<string> grep_search(path const &start, string const &pattern) {
    auto finder = stream_finder(pattern);
    unordered_map<ino_t, path> dist;
    unordered_set<ino_t> checked;
    vector<ino_t> good;
    set<pair<path, ino_t>> heap;

    ino_t beg = get_stat(start).inod;
    dist[beg] = path();
    heap.insert({path(), beg});

    while (!heap.empty()) {
        path cur_path = heap.begin()->first;
        heap.erase(heap.begin());
        autocloseable_dir cur_dir;
        try {
            cur_dir = autocloseable_dir(start.resolve(cur_path));
        } catch (runtime_error const &e) {
            print_error(e.what());
            continue;
        }

        dirent *it;
        while (it = cur_dir.next_entry()) {
            string name(it->d_name);
            if (name == "." || name == "..") {
                continue;
            }
            path next_path = cur_path.add_to_path(name);
            my_stat next_stats;
            try {
                next_stats = get_stat(start.resolve(next_path));
            } catch (runtime_error const &e) {
                print_error(e.what());
                continue;
            }

            bool updated = 0;

            if (!dist.count(next_stats.inod)) {
                dist[next_stats.inod] = next_path;
                updated = 1;
            } else {
                if (next_path < dist[next_stats.inod]) {
                    heap.erase({dist[next_stats.inod], next_stats.inod});
                    dist[next_stats.inod] = next_path;
                    updated = 1;
                }
            }

            if (S_ISDIR(next_stats.mode) && updated) {
                heap.insert({next_path, next_stats.inod});
            } else if (S_ISREG(next_stats.mode) && !checked.count(next_stats.inod)) {
                if (check_file(start.resolve(next_path), finder, static_cast<size_t>(next_stats.buff_size))) {
                    good.emplace_back(next_stats.inod);
                }
                checked.insert(next_stats.inod);
            }
        }
    }
    vector<string> res;
    for (auto id : good) {
        res.push_back(dist[id]);
    }
    return res;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "One argument expected" << endl;
        return 1;
    }

    path start_path;
    try {
        start_path = get_cur_dir();
    } catch (runtime_error const &e) {
        print_error(e.what());
        return 1;
    }
    auto res = grep_search(start_path, argv[1]);
    for (auto &s : res) {
        cout << s << endl;
    }
    return 0;
}
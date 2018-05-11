#include "autocloseable.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>

using std::runtime_error;

autocloseable_file::autocloseable_file() noexcept : fd(-1) {}

autocloseable_file::autocloseable_file(std::string const &f_path, int flags): fd(open(f_path.c_str(), flags)) {
    if (fd == -1) {
        throw runtime_error("Can't open file: " + f_path + "\n");
    }
}

autocloseable_file::autocloseable_file(autocloseable_file &&other) noexcept : fd(other.fd) {
    other.fd = -1;
}

autocloseable_file& autocloseable_file::operator=(autocloseable_file &&other) noexcept {
    fd = other.fd;
    other.fd = -1;
    return *this;
}

ssize_t autocloseable_file::read_data(void* buf, size_t count) {
    ssize_t res = read(fd, buf, count);
    if (res == -1) {
        throw runtime_error("Error occurred during reading file\n");
    }
    return res;
}

autocloseable_file::~autocloseable_file() {
    if (fd >= 0) {
        close(fd);
    }
}

autocloseable_dir::autocloseable_dir() noexcept : dir(nullptr) {}

autocloseable_dir::autocloseable_dir(std::string const &d_path) : dir(opendir(d_path.c_str())) {
    if (dir == nullptr) {
        throw runtime_error("Can't open directory: " + d_path + "\n");
    }
}

autocloseable_dir::autocloseable_dir(autocloseable_dir &&other) noexcept : dir(other.dir) {
    other.dir = nullptr;
}

autocloseable_dir& autocloseable_dir::operator=(autocloseable_dir &&other) noexcept {
    dir = other.dir;
    other.dir = nullptr;
    return *this;
}

dirent* autocloseable_dir::next_entry() {
    return readdir(dir);
}

autocloseable_dir::~autocloseable_dir() {
    if (dir != nullptr) {
        closedir(dir);
    }
}
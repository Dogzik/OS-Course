//
// Created by dogzik on 10.05.18.
//

#ifndef HW2_AUTOCLOSABLE_H
#define HW2_AUTOCLOSABLE_H

#include "path.h"
#include <dirent.h>

struct autocloseable_file {
    autocloseable_file() noexcept;

    autocloseable_file(std::string const& f_path, int flags);
    autocloseable_file(autocloseable_file const&) = delete;
    autocloseable_file(autocloseable_file&& other) noexcept;

    autocloseable_file& operator=(autocloseable_file const&) = delete;
    autocloseable_file& operator=(autocloseable_file&& other) noexcept;

    ssize_t read_data(void* buf, size_t count);

    ~autocloseable_file();
private:
    int fd;
};


struct autocloseable_dir {
    autocloseable_dir() noexcept;

    explicit autocloseable_dir(std::string const& d_path);
    autocloseable_dir(autocloseable_dir const&) = delete;
    autocloseable_dir(autocloseable_dir&& other) noexcept;

    autocloseable_dir& operator=(autocloseable_dir const&) = delete;
    autocloseable_dir& operator=(autocloseable_dir&& other) noexcept;

    dirent* next_entry();

    ~autocloseable_dir();
private:
    DIR* dir;
};




#endif //HW2_AUTOCLOSABLE_H

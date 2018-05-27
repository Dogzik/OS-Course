//
// Created by dogzik on 27.05.18.
//

#ifndef HW6_UTILS_H
#define HW6_UTILS_H

#include <string>
#include "stream_socket.h"

std::string error_msg(std::string const& annotation);

struct querry_accumulator {
    std::string next_querry();

    void add_data(std::string const& new_data);

    bool empty();
private:
    std::string data;
};

void add_to_epoll(int epoll_fd, int fd, uint32_t events);
void change_epoll_mod(int epoll_fd, int fd, uint32_t events);
void erase_from_epoll(int epoll_fd, int fd);
#endif //HW6_UTILS_H

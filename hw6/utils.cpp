//
// Created by dogzik on 27.05.18.
//
#include "utils.h"
#include "stream_socket.h"
#include <cstring>
#include <errno.h>
#include <sys/epoll.h>

using std::string;

string error_msg(string const& annotation) {
    return annotation + ":\n" + strerror(errno);
}

void querry_accumulator::add_data(string const &new_data) {
    data += new_data;
}

bool querry_accumulator::empty() {
    return data.empty();
}

string querry_accumulator::next_querry() {
    size_t end_ind = data.find("\r\n");
    if (end_ind == string::npos) {
        return "";
    }
    string res = data.substr(0, end_ind);
    data = data.substr(end_ind + 2);
    return res;
}

void add_to_epoll(int epoll_fd, int fd, uint32_t events) {
    struct epoll_event event{};
    event.events = events;
    event.data.fd = fd;
    if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        throw std::runtime_error(error_msg("Can't add fd to epoll"));
    }
}

void change_epoll_mod(int epoll_fd, int fd, uint32_t events) {
    struct epoll_event event{};
    event.events = events;
    event.data.fd = fd;
    if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event) == -1) {
        throw std::runtime_error(error_msg("Can't change fd's epoll mod"));
    }
}

void erase_from_epoll(int epoll_fd, int fd) {
    if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        throw std::runtime_error(error_msg("Can't erase fd from epoll"));
    }
}
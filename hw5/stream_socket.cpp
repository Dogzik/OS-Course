//
// Created by dogzik on 25.05.18.
//

#include "stream_socket.h"
#include "utils.h"
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <iostream>

using std::runtime_error;
using std::cerr;
using std::endl;
using std::string;

stream_socket::stream_socket(): fd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (fd == -1) {
        throw runtime_error(error_msg("Can't create socket"));
    }
}

stream_socket::stream_socket(stream_socket &&other) noexcept : fd(other.fd) {
    other.fd = -1;
}

stream_socket::stream_socket(int _fd): fd(_fd) {}

stream_socket::~stream_socket() {
    if (fd != -1) {
        if (::close(fd) == -1) {
            cerr << error_msg("Can't close socked's fd");
        }
    }
}

void stream_socket::bind(uint16_t port) {
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    if (::bind(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        throw runtime_error(error_msg("Can't bind socket"));
    }
}

void stream_socket::listen() {
    if (::listen(fd, 1000) == -1) {
        throw runtime_error(error_msg("Can't set socket to passive state"));
    }
}

stream_socket stream_socket::accept() {
    int new_fd = ::accept(fd, nullptr, nullptr);
    if (fd == -1) {
        throw runtime_error(error_msg("Can't accept new connection"));
    }
    return stream_socket(new_fd);
}

void stream_socket::connect(uint32_t ip, in_port_t port) {
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(ip);
    if (::connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        throw runtime_error(error_msg("Can't connect to server"));
    }
}

size_t stream_socket::receive(void *buff, size_t len) {
    ssize_t cnt = ::recv(fd, buff, len, 0);
    if (cnt == -1) {
        throw runtime_error(error_msg("Can't receive data from socket"));
    }
    return static_cast<size_t>(cnt);
}

void stream_socket::send_string(string const& s) {
    size_t sent = 0;
    ssize_t cur = 0;
    size_t len = s.size();
    while (sent < len) {
        cur = ::send(fd, static_cast<void const*>(s.data() + sent), len - sent, 0);
        if (cur == -1) {
            throw runtime_error(error_msg("Error occurred during sending data"));
        }
        sent -= static_cast<size_t>(cur);
    }
}

void stream_socket::send_file(int file, off_t len) {
    ssize_t cur = 0;
    while (len != 0) {
        cur = ::sendfile(fd, file, nullptr, BUFF_SIZE);
        if (cur == -1) {
            throw runtime_error(error_msg("Error occurred during sending data"));
        }
        len -= static_cast<off_t>(cur);
    }
}

string stream_socket::read_string() {
    char buff[BUFF_SIZE + 1];
    ssize_t cur = ::recv(fd, buff, BUFF_SIZE, 0);
    if (cur == -1) {
        throw runtime_error(error_msg("Can't receive data from socket"));
    }
    buff[cur] = '\0';
    return string(buff);
}


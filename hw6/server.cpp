//
// Created by dogzik on 27.05.18.
//

#include <iostream>
#include <memory>
#include <unordered_map>
#include <unistd.h>
#include <sys/epoll.h>
#include "stream_socket.h"
#include "utils.h"

using std::cerr;
using std::endl;
using std::string;
using std::cout;
using std::endl;
using std::unordered_map;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "1 argument expected: <porn>" << endl;
        return 1;
    }
    in_port_t port;
    try {
        port = static_cast<in_port_t>(std::stoi(argv[1]));
    } catch (...) {
        cerr << "Incorrect port argument" << endl;
        return 1;
    }

    int epoll_fd = ::epoll_create(1000);
    if (epoll_fd == -1) {
        cerr << error_msg("Can'r create epoll_fd") << endl;
        return 1;
    }
    auto epoll_closer = std::unique_ptr<int, void(*)(int*)>(&epoll_fd, [](int* f){::close(*f);});

    unordered_map<int, stream_socket> socks;
    int listener_fd = -1;
    try {
        stream_socket listener;
        listener.bind(port);
        listener.listen();
        listener_fd = listener.get_fd();
        add_to_epoll(epoll_fd, listener_fd, EPOLLIN);
        add_to_map(socks, std::move(listener));
    } catch (std::runtime_error const& e) {
        cerr << e.what() << endl;
        return 1;
    }

    int const MAX_EVENTS = 200;
    auto events_ptr = std::unique_ptr<struct epoll_event, void(*)(struct epoll_event*)>(
            new struct epoll_event[MAX_EVENTS],
                    [](struct epoll_event* p){delete[](p);});

    unordered_map<int, querry_accumulator> received_data;
    unordered_map<int, string> data_to_send;
    try {
        while (1) {
            int cnt = ::epoll_wait(epoll_fd, events_ptr.get(), MAX_EVENTS, -1);
            if (cnt == -1) {
                cerr << error_msg("Epoll_wait failed") << endl;
                return 1;
            }
            for (int i = 0; i < cnt; ++i) {
                uint32_t events = events_ptr.get()[i].events;
                int fd = events_ptr.get()[i].data.fd;
                if (events & (EPOLLERR | EPOLLHUP)) {
                    erase_from_epoll(epoll_fd, fd);
                    received_data.erase(fd);
                    data_to_send.erase(fd);
                    continue;
                }
                if (events & EPOLLIN) {
                    if (fd == listener_fd) {
                        stream_socket new_client = socks[fd].accept();
                        add_to_epoll(epoll_fd, new_client.get_fd(), EPOLLIN);
                        add_to_map(socks, std::move(new_client));
                    } else {
                        received_data[fd].add_data(socks[fd].read_string());
                        string request;
                        while (!(request = received_data[fd].next_querry()).empty()) {
                            if (!data_to_send.count(fd)) {
                                change_epoll_mod(epoll_fd, fd, EPOLLIN | EPOLLOUT);
                            }
                            data_to_send[fd] += "Hello, " + request + "\r\n";
                        }
                    }
                }
                if (events & EPOLLOUT) {
                    data_to_send[fd] = socks[fd].send_string(data_to_send[fd]);
                    if (data_to_send[fd].empty()) {
                        data_to_send.erase(fd);
                        change_epoll_mod(epoll_fd, fd, EPOLLIN);
                    }
                }
            }
        }
    } catch (std::runtime_error const& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
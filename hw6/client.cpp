//
// Created by dogzik on 27.05.18.
//

#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <netdb.h>
#include <sys/epoll.h>
#include <errno.h>
#include <memory>
#include <unistd.h>
#include "utils.h"
#include "stream_socket.h"

using std::cerr;
using std::endl;
using std::string;
using std::unordered_map;

string read_from_stdin() {
    char buff[stream_socket::BUFF_SIZE + 1];
    ssize_t cur = ::read(STDIN_FILENO, static_cast<void*>(buff), stream_socket::BUFF_SIZE);
    if (cur == -1) {
        throw std::runtime_error(error_msg("Failed to read from stdin"));
    }
    buff[cur] = '\0';
    return string(buff);
}

string write_to_stdout(string const& s) {
    ssize_t cur = ::write(STDOUT_FILENO, static_cast<void const*>(s.data()), s.length());
    if (cur == -1) {
        throw std::runtime_error(error_msg("Failed to write to stdout"));
    }
    return s.substr(static_cast<size_t>(cur));
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "2 arguments expected: <host> <porn>" << endl;
        return 1;
    }
    struct hostent* hp = gethostbyname(argv[1]);
    if (hp == nullptr) {
        cerr << "No such host found: " << argv[1] << endl;
        return 1;
    }
    uint32_t ip = ntohl(reinterpret_cast<struct in_addr*>(hp->h_addr_list[0])->s_addr);
    in_port_t port;

    try {
        port = static_cast<in_port_t >(std::stoi(argv[2]));
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

    try {
        stream_socket server;
        server.make_nonblock();
        add_to_epoll(epoll_fd, STDIN_FILENO, EPOLLIN);
        bool connected = 1;
        bool stop = 0;

        if (server.connect(ip, port) == -1) {
            if (errno == EINPROGRESS) {
                add_to_epoll(epoll_fd, server.get_fd(), EPOLLIN | EPOLLOUT);
                connected = 0;
            } else {
                throw std::runtime_error(error_msg("Connection failed"));
            }
        } else {
            add_to_epoll(epoll_fd, server.get_fd(), EPOLLIN);
        }

        querry_accumulator received_data;
        string data_to_print;
        string data_to_send;

        int const MAX_EVENTS = 3;
        auto events_ptr = std::unique_ptr<struct epoll_event, void (*)(struct epoll_event *)>(
                new struct epoll_event[MAX_EVENTS],
                        [](struct epoll_event *p) { delete[](p); });

        while (!stop) {
            int cnt = ::epoll_wait(epoll_fd, events_ptr.get(), MAX_EVENTS, -1);
            if (cnt == -1) {
                cerr << error_msg("Epoll_wait failed") << endl;
                return 1;
            }
            for (int i = 0; i < cnt; ++i) {
                uint32_t events = events_ptr.get()[i].events;
                int fd = events_ptr.get()[i].data.fd;
                if (fd == server.get_fd()) {
                    if ((events & EPOLLERR) || (connected && (events & EPOLLHUP))) {
                        cerr << "Server disconnected" << endl;
                        return 1;
                    }
                    if (events & EPOLLIN) {
                        received_data.add_data(server.read_string());
                        string answer;
                        while (!(answer = received_data.next_querry()).empty()) {
                            if (data_to_print.empty()) {
                                add_to_epoll(epoll_fd, STDOUT_FILENO, EPOLLOUT);
                            }
                            data_to_print += answer;
                        }
                    }
                    if (events & EPOLLOUT) {
                        if (!connected) {
                            int err = 0;
                            socklen_t len = sizeof(int);

                            if (::getsockopt(server.get_fd(), SOL_SOCKET, SO_ERROR, &err, &len) == -1) {
                                throw std::runtime_error(error_msg("Can't get socket errors status"));
                            }
                            if (err != 0) {
                                throw std::runtime_error(string("Connection failed:\n") + strerror(err));
                            }
                            connected = 1;
                            change_epoll_mod(epoll_fd, fd, EPOLLIN | (data_to_send.empty() ? 0 : EPOLLOUT));
                        } else {
                            data_to_send = server.send_string(data_to_send);
                            if (data_to_send.empty()) {
                                change_epoll_mod(epoll_fd, fd, EPOLLIN);
                            }
                        }
                    }
                } else if (fd == STDIN_FILENO) {
                    string request = read_from_stdin();
                    if (request.empty()) {
                        stop = 1;
                        continue;
                    }
                    if (data_to_send.empty()) {
                        change_epoll_mod(epoll_fd, server.get_fd(), EPOLLIN | EPOLLOUT);
                    }
                    data_to_send += request + "\r\n";
                } else if (fd == STDOUT_FILENO) {
                    data_to_print = write_to_stdout(data_to_print);
                    if (data_to_print.empty()) {
                        erase_from_epoll(epoll_fd, STDOUT_FILENO);
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

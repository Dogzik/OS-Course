//
// Created by dogzik on 27.05.18.
//

#ifndef HW6_STREAM_SOCKET_H
#define HW6_STREAM_SOCKET_H

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <unordered_map>

struct stream_socket {
    static constexpr size_t BUFF_SIZE = 2048;

    stream_socket();

    stream_socket(stream_socket const&) = delete;
    stream_socket(stream_socket&& other) noexcept;

    stream_socket& operator=(stream_socket const&) = delete;
    stream_socket& operator=(stream_socket&& other) noexcept;

    std::string read_string();
    std::string send_string(std::string const& s);

    //server
    void bind(in_port_t port);
    void listen(int max_connections = 1000);
    stream_socket accept();

    //client
    void connect(uint32_t ip, in_port_t port);

    int get_fd();

    ~stream_socket();

    friend void add_to_map(std::unordered_map<int, stream_socket>& socks, stream_socket&& sock);
private:
    int fd;

    explicit stream_socket(int _fd);
};

void add_to_map(std::unordered_map<int, stream_socket>& socks, stream_socket&& sock);
#endif //HW6_STREAM_SOCKET_H

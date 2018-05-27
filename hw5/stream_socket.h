//
// Created by dogzik on 25.05.18.
//

#ifndef HW5_STREAM_SOCKET_H
#define HW5_STREAM_SOCKET_H

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdint>
#include <netinet/in.h>
#include <string>

struct stream_socket {
    static constexpr size_t BUFF_SIZE = 2048;

    stream_socket();

    stream_socket(stream_socket const&) = delete;
    stream_socket(stream_socket&& other) noexcept;

    stream_socket& operator=(stream_socket const&) = delete;
    stream_socket& operator=(stream_socket&& other) noexcept;

    size_t receive(void *buff, size_t len);
    std::string read_string();
    void send_string(std::string const& s);
    void send_file(int file, off_t len);

    //server
    void bind(uint16_t port);
    void listen();
    stream_socket accept();

    //client
    void connect(uint32_t ip, in_port_t port);

    ~stream_socket();
private:
    int fd;


    explicit stream_socket(int _fd);
};


#endif //HW5_STREAM_SOCKET_H

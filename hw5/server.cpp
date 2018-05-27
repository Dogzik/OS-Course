//
// Created by dogzik on 26.05.18.
//
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include "stream_socket.h"
#include "utils.h"

using std::string;
using std::istringstream;
using std::transform;
using std::to_string;
using std::cout;
using std::endl;
using std::unique_ptr;
using std::cerr;

string get_file_stat(string const& name) {
    struct stat data{};
    if (::stat(name.c_str(), &data) == -1) {
        return "No such file";
    }
    if (!S_ISREG(data.st_mode)) {
        return "Not a regular file";
    }
    return "File size is: " + to_string(data.st_size) + " byte(s)";

}


bool process(string& querry, stream_socket& sock, querry_accumulator& acc) {
    auto iss = istringstream(querry);
    string type;
    iss >> type;
    if (type == "get") {
        string name;
        iss >> name;
        struct stat data{};
        if ((::stat(name.c_str(), &data) == -1) || (!S_ISREG(data.st_mode)) || (!(data.st_mode & S_IROTH))) {
            sock.send_string("ERR\r\n");
            return 0;
        }
        sock.send_string("OK " + std::to_string(data.st_size) + "\r\n");
        type = read_until_end(acc, sock);
        if (type == "yes") {
            int fd = ::open(name.c_str(), O_RDONLY);
            auto ptr = unique_ptr<int, void(*)(int*)>(&fd, [](int* f){::close(*f);});
            sock.send_file(fd, data.st_size);
        }
        return 0;
    } else if (type == "stat") {
        string name;
        iss >> name;
        sock.send_string(get_file_stat(name) + "\r\n");
        return 0;
    } else if (type == "exit") {
        return 1;
    } else {
        sock.send_string("No such command\r\n");
        return 0;
    }
}

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
    try {
        stream_socket listener;
        listener.bind(port);
        listener.listen();
        while (1) {
            stream_socket client = listener.accept();
            client.send_string("Successfully connected\r\n");
            querry_accumulator acc;
            bool stop = 0;
            while (!stop) {
                string querry = read_until_end(acc, client);
                if (querry.empty()) {
                    stop = 1;
                    continue;
                }
                stop = process(querry, client, acc);
            }
        }
    } catch (std::runtime_error const& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

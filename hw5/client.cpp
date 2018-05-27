//
// Created by dogzik on 26.05.18.
//

#include <netdb.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "stream_socket.h"
#include "utils.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::cerr;


int main(int argc, char *argv[]) {
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

    try {
        stream_socket server;
        server.connect(ip, port);
        cout << "Connecting to server..." << endl;
        querry_accumulator hello_acc;
        cout << read_until_end(hello_acc, server) << endl;

        string request;
        while (getline(std::cin, request)) {
            auto iss = std::istringstream(request);
            string type;
            iss >> type;
            if (type == "exit") {
                server.send_string("exit\r\n");
                break;
            } else if (type == "stat") {
                string s;
                iss >> s;
                server.send_string("stat " + s + "\r\n");
                querry_accumulator acc;
                cout << read_until_end(acc, server) << endl;
            } else if (type == "get") {
                string from, to;
                querry_accumulator acc;
                iss >> from >> to;
                server.send_string("get " + from + "\r\n");
                string res = read_until_end(acc, server);
                if (res == "ERR") {
                    cout << "No such file on server" << endl;
                    continue;
                }
                size_t sz = std::stoull(res.substr(3));
                cout << "Would you like to receive " << sz << " byte(s)?" << endl;
                getline(cin, request);
                if (request == "yes") {
                    server.send_string("yes\r\n");
                    auto out = std::ofstream(to, std::ios_base::binary);
                    char buff[stream_socket::BUFF_SIZE];
                    size_t cur = 0;
                    while (sz > 0) {
                        cur = server.receive(static_cast<void*>(buff), stream_socket::BUFF_SIZE);
                        out.write(buff, cur);
                        sz -= cur;
                    }
                    cout << from << " successfully written to " << to << endl;
                }
            } else {
                cout << "No such command" << endl;
            }
        }
    } catch (std::runtime_error const& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
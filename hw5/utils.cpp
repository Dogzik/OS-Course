//
// Created by dogzik on 25.05.18.
//

#include "utils.h"
#include "stream_socket.h"
#include <cstring>
#include <errno.h>

using std::string;


string error_msg(string const& annotation) {
    return annotation + ":\n" + strerror(errno);
}

void querry_accumulator::add_data(string const &new_data) {
    data += new_data;
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

string read_until_end(querry_accumulator &acc, stream_socket &sock) {
    string res;
    string cur;
    while ((res = acc.next_querry()).empty() && !(cur = sock.read_string()).empty()) {
        acc.add_data(cur);
    }
    return res;
}
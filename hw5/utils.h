//
// Created by dogzik on 25.05.18.
//

#ifndef HW5_UTILS_H
#define HW5_UTILS_H

#include <string>
#include "stream_socket.h"

std::string error_msg(std::string const& annotation);

struct querry_accumulator {
    std::string next_querry();

    void add_data(std::string const& new_data);

private:
    std::string data;
};

std::string read_until_end(querry_accumulator &acc, stream_socket &sock);

#endif //HW5_UTILS_H

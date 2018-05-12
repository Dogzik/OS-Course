//
// Created by dogzik on 12.05.18.
//

#include "utils.h"
#include <iostream>
#include <errno.h>
#include <cstring>

using std::string;
using std::cerr;
using std::endl;

void print_error(string const& msg) {
    cerr << msg << endl << strerror(errno) << endl;
}
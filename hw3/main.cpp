#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include "runtime_function.h"
#include "utils.h"

using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::unique_ptr;
using std::make_unique;
using std::bad_alloc;
using std::string;
using std::runtime_error;
using std::istringstream;

byte const CODE[] = {
        0x69, 0xc7, 0xc8, 0x00, 0x00, 0x00,
        0xc3
};

size_t const POS = 2;

/*
 * Simple runtime function, thant multiplies given number to constant unsigned char in memory
 * Type "exec x" to execute function with "x"
 * Type "change y" to change stored constant to "y"
 * Type "exit" to exit
 */
int main() {
    vector<byte> code(CODE, CODE + sizeof(CODE));
    unique_ptr<runtime_function> f_ptr;
    try {
        f_ptr = make_unique<runtime_function>(code);
    } catch (bad_alloc const& e) {
        print_error("Unable to allocate memory");
        return 1;
    }

    string input;
    while(getline(cin, input)) {
        string task;
        int value;
        auto iss = istringstream(input);
        iss >> task;
        try {
            if (task == "exec") {
                iss >> value;
                cout << "Result: " << f_ptr->execute<int>(value) << endl;
            } else if (task == "change") {
                iss >> value;
                f_ptr->set_byte(POS, static_cast<byte>(value));
                cout << "New multiplier: " << static_cast<int>(static_cast<byte>(value)) << endl;
            } else if (task == "exit"){
                break;
            } else {
                cout << "Unknown command" << endl;
            }
        } catch (runtime_error const& e) {
            print_error(e.what());
            return 1;
        }
    }
    return 0;
}
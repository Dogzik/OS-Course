#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstring>

using std::string;
using std::istringstream;
using std::cin;
using std::cout;
using std::getline;
using std::unique_ptr;
using std::vector;
using std::move;
using std::endl;
using std::cerr;

void greet() {
    cout << "~$ ";
    cout.flush();
}

void print_error(string const& msg) {
    cerr << msg << endl << strerror(errno) << endl;
}

vector<string> string_args(string const& s) {
    auto iss = istringstream(s);
    vector<string> res;
    string tmp;
    while (iss >> tmp) {
        res.emplace_back(move(tmp));
    }
    return res;
}

char** raw_args(vector<string>& args) {
    size_t cnt = args.size();
    auto res = new char* [cnt + 1];
    res[cnt] = nullptr;
    for (size_t i = 0; i < cnt; ++i) {
        res[i] = const_cast<char*>(args[i].data());
    }
    return res;
}

void execute(char* args[], char* envp[]) {
    pid_t pid = fork();
    if (pid == 0) {
        if (execve(args[0], args, envp) == -1) {
            print_error("Execution failed");
            exit(-1);
        }
    } else if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            print_error("Error occurred during execution");
        } else {
            cout << "Program returned with code: " << WEXITSTATUS(status) << endl;
        }
    } else {
        print_error("Fork failed");
    }
}

int main(int arc, char* argv[], char* envp[]) {
    greet();
    string input;
    while (getline(cin, input)) {
        if (input == "exit") {
            break;
        }
        vector<string> args = string_args(input);
        auto ptr = unique_ptr<char*[]>(raw_args(args));
        execute(ptr.get(), envp);
        greet();
    }
    cout << "exiting..." << endl;
    return 0;
}
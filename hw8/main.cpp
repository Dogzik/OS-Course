#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <setjmp.h>
#include <iostream>

#define PRINT_REG(NAME) \
write_str(#NAME" = ");\
write_str(ull_to_string(static_cast<ull>(mcontext->gregs[REG_##NAME]), number_buff));\
write_str("\n");\

using ull = uint64_t;

sigjmp_buf jmpbuf;

inline void write_str(char const* s) {
    ssize_t cur = 0;
    ssize_t len = static_cast<ssize_t>(strlen(s));
    ssize_t tmp;
    while (cur < len) {
        tmp = write(STDERR_FILENO, s + cur, len - cur);
        if (tmp == -1) {
            exit(errno);
        }
        cur += tmp;
    }
}

inline char get_digit(ull x) {
    if (x < 10) {
        return static_cast<char>('0' + x);
    }
    switch (x) {
        case 10:
            return 'a';
        case 11:
            return 'b';
        case 12:
            return 'c';
        case 13:
            return 'd';
        case 14:
            return 'e';
        case 15:
            return 'f';
        default:
            exit(45);
    }
}

inline char* ull_to_string(ull a, char* buff) {
    buff[0] = '0';
    buff[1] = 'x';
    for (size_t i = 0; i < 16; ++i) {
        buff[2 + i] = get_digit(a % 16);
        a = a / 16;
    }
    buff[18] = '\0';
    return buff;
}

inline char* byte_to_string(uint8_t a, char* buff) {
    buff[0] = get_digit(a % 16);
    buff[1] = get_digit(a / 16);
    buff[2] = '\0';
    return buff;
}

inline void write_reg(mcontext_t* mcontext, size_t reg, char* buff) {
    write_str(ull_to_string(static_cast<ull>(mcontext->gregs[reg]), buff));
}

void internal_handler(int signum) {
    siglongjmp(jmpbuf, 1337);
}

void global_handler(int signum, siginfo_t* siginfo, void* context) {
    write_str("==========================\n");
    write_str("    Segmnetatin fault     \n");
    write_str("==========================\n");
    char number_buff[19];
    write_str("Address = ");
    ull addr = reinterpret_cast<ull>(siginfo->si_addr);
    write_str(ull_to_string(addr, number_buff));
    write_str("\n\n");

    mcontext_t* mcontext = &reinterpret_cast<ucontext_t*>(context)->uc_mcontext;

    PRINT_REG(R8)
    PRINT_REG(R9)
    PRINT_REG(R10)
    PRINT_REG(R11)
    PRINT_REG(R13)
    PRINT_REG(R14)
    PRINT_REG(R15)
    PRINT_REG(RDI)
    PRINT_REG(RSI)
    PRINT_REG(RBP)
    PRINT_REG(RBX)
    PRINT_REG(RDX)
    PRINT_REG(RAX)
    PRINT_REG(RCX)
    PRINT_REG(RSP)

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = internal_handler;
    sa.sa_flags = SA_NODEFER;
    if (sigaction(SIGSEGV, &sa, nullptr) == -1) {
        exit(errno);
    }

    write_str("\nMemory dump:\n");

    int cnt = 0;
    ull start = (addr & ~static_cast<ull>(15));
    start = (start - 64) <= start ? (start - 64) : 0;
    for (ull i = start; cnt < 9; i += 16, ++cnt) {
        bool f = 0;
        for (ull j = i; j < i + 16; ++j) {
            if (j == addr) {
                write_str("[");
                f = 1;
            } else {
                if (f) {
                    write_str("]");
                    f = 0;
                } else {
                    write_str(" ");
                }
            }

            if (sigsetjmp(jmpbuf, 0)) {
                write_str("**");
            } else {
                uint8_t byte = *reinterpret_cast<uint8_t*>(j);
                write_str(byte_to_string(byte, number_buff));
            }
        }
        if (f) {
            write_str("]");
        }
        write_str("\n");
    }
    exit(46);
}


int main() {
    struct sigaction new_action;
    memset(&new_action, 0, sizeof(new_action));
    new_action.sa_sigaction = global_handler;
    new_action.sa_flags = SA_SIGINFO | SA_NODEFER;
    if (sigaction(SIGSEGV, &new_action, nullptr) == -1) {
        exit(errno);
    }
    int* a = new int[20];
    for (int i = 0; 1; ++i) {
        a[i] = i + 1337;
        std::cout << a[i] << std::endl;
    }
    return 0;
}


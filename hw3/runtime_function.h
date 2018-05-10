#ifndef HW3_RUNTIME_FUNCTION_H
#define HW3_RUNTIME_FUNCTION_H

#include <vector>
#include <new>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <sys/mman.h>

using byte = unsigned char;

struct runtime_function {
    explicit runtime_function(std::vector<byte> const& code);

    void set_byte(size_t pos, byte one);

    template<typename R, typename ...Args>
    R execute(Args&& ...args) {
        change_priv(PROT_EXEC | PROT_READ);
        auto func = reinterpret_cast<R(*)(std::decay_t<Args>...)>(data);
        return func(std::forward<Args>(args)...);
    };

    ~runtime_function();

private:
    void* data;
    size_t size;
    int priv;

    void change_priv(int n_priv);
};

#endif //HW3_RUNTIME_FUNCTION_H

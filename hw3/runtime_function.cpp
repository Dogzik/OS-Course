#include "runtime_function.h"

bool runtime_function::change_priv(int n_priv) {
    if (priv == n_priv) {
        return 1;
    }
    if (mprotect(data, size, n_priv) == -1) {
        return 0;
    } else {
        priv = n_priv;
        return 1;
    }
}

void runtime_function::try_change_priv(int n_priv) {
    if (!change_priv(n_priv)) {
        throw std::runtime_error("Can't change privileges");
    }
}

void runtime_function::set_byte(size_t pos, byte one) {
    assert(pos < size);
    try_change_priv(PROT_WRITE | PROT_READ);
    static_cast<byte*>(data)[pos] = one;
}

runtime_function::runtime_function(std::vector<byte> const& code): size(code.size()), priv(PROT_WRITE | PROT_READ) {
    data = mmap(nullptr, size, priv, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (data == MAP_FAILED) {
        throw std::bad_alloc();
    }
    std::memcpy(data, code.data(), size);
}

runtime_function::~runtime_function() {
    if (munmap(data, size) == -1) {
        throw std::bad_alloc();
    }
}
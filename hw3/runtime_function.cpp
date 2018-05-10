#include "runtime_function.h"

void runtime_function::change_priv(int n_priv) {
    if (priv == n_priv) {
        return;
    }
    if (mprotect(data, size, n_priv) == -1) {
        throw std::runtime_error("Can't change privileges");
    }
    priv = n_priv;
}

void runtime_function::set_byte(size_t pos, byte one) {
    assert(pos < size);
    change_priv(PROT_WRITE | PROT_READ);
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
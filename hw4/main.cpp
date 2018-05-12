#include <dlfcn.h>
#include <iostream>
#include <memory>

#include "mul.h"
#include "add.h"
#include "sub.h"

using namespace std;

int main() {
	cout << add(1337, 1488) << endl;
	cout << mul(228, 2517) << endl;

	auto extern_lib = unique_ptr<void, void(*)(void*)>(dlopen("libs/libdynamic_loadable.so", RTLD_LAZY), [](void* p){dlclose(p);});
	if (extern_lib.get() == nullptr) {
		cerr << dlerror() << endl;
		return 1;
	}
	using sub_t = int(*)(int, int);
	auto sub_f = reinterpret_cast<sub_t>(dlsym(extern_lib.get(), "_Z3subii"));
	if (sub_f == nullptr) {
		cerr << dlerror() << endl;
		return 1;
	}
	cout << sub_f(4, 1) << endl;
	return 0;
}

.PHONY: all clean run

all: main

run: all
	./main

main: main.o 
	g++ -std=c++14 -o main -L./libs -Wl,-rpath=./libs main.o -lstatic -ldynamic_shared -ldl
	rm -rf *.o

main.o: main.cpp ../libs/libdynamic_shared.so ../libs/libdynamic_loadable.so ../libs/libstatic.a
	g++ -std=c++14 -I./include -c -o $@ $<

../libs/libdynamic_shared.so: make_shared.sh
	chmod +x $<
	./$<

../libs/libdynamic_loadable.so: make_loadable.sh
	chmod +x $<
	./$<

../libs/libstatic.a: make_static.sh
	chmod +x $<
	./$<

clean:
	rm main


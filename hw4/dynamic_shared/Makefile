.PHONY: all clean

all: ../libs/libdynamic_shared.so

../libs/libdynamic_shared.so: add.o
	g++ -std=c++14 -shared -o $@ $^
	rm -rf *.o
	cp *.h ../include

%.o: %.cpp
	g++ -std=c++14 -fPIC -c -o $@ $^

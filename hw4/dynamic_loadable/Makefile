.PHONY: all clean

all: ../libs/libdynamic_loadable.so

../libs/libdynamic_loadable.so: sub.o
	g++ -std=c++14 -shared -o $@ $^
	rm -rf *.o
	cp *.h ../include

%.o: %.cpp
	g++ -std=c++14 -fPIC -c -o $@ $^

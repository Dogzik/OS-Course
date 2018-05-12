.PHONY: all

all: libstatic.a

libstatic.a: mul.o
	ar rcs ../libs/$@ $^
	rm -rf *.o
	cp *.h ../include

%.o: %.cpp
	g++ -std=c++14 -c -o $@ $^


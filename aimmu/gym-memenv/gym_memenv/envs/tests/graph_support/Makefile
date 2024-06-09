CC=g++
CFLAGS=-std=c++11 -ggdb

test: test.cpp data_collect.cpp
	${CC} ${CFLAGS} -o test test.cpp data_collect.cpp

clean:
	rm -rf *.o  test

CC:=/usr/local/intel/bin/icpc

all:	test.cc
	$(CC) -g -Wall -std=c++1y -O3 -march=native -mtune=native test.cc -lbenchmark -lpthread -ltbb -o test

debug:	test.cc
	$(CC) -g -Wall -std=c++1y -O0 -march=native -mtune=native test.cc -lbenchmark -lpthread -ltbb -o test

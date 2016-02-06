CC:=/usr/local/intel/bin/icpc

all:	test.cc
	$(CC) -g -Wall --std=c++11 -O3 -march=native -mtune=native test.cc -lbenchmark -lpthread -o test -I/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/ -L/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/intelpcm.so/ -Wl,-rpath=/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/intelpcm.so/  -lintelpcm

debug:	test.cc
	$(CC) -g -Wall --std=c++11 -O0 test.cc -lbenchmark -lpthread -o test -I/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/ -L/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/intelpcm.so/ -Wl,-rpath=/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/intelpcm.so/  -lintelpcm

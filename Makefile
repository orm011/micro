all:	test.cc
	clang++ --std=c++14 -O3 test.cc -lbenchmark -lpthread -o test -I/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/ -L/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/intelpcm.so/ -Wl,-rpath=/home/orm/IntelPerformanceCounterMonitor-PCM-V2.10/intelpcm.so/  -lintelpcm

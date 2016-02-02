#include <benchmark/benchmark.h>
#include <string.h>
#include <unistd.h>
#include <cpucounters.h>
#include <iostream>
#include <immintrin.h>

//_mm256_stream_si256 
using namespace std;

void
streams
(
 size_t num_columns,
 size_t column_size,
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ results,
 int concurrent
)
{
  if (concurrent) {
  for (size_t i = 0; i < column_size; ++i ) {
      results[0][i] = input_data[0][i]; // gather
      
      results[1][i] = input_data[1][i]; // gather

      results[2][i] = input_data[2][i]; // gather

      results[3][i] = input_data[3][i]; // gather

      results[4][i] = input_data[4][i]; // gather

      results[5][i] = input_data[5][i]; // gather

      results[6][i] = input_data[6][i]; // gather

      results[7][i] = input_data[7][i]; // gather

      results[8 + 0][i] = input_data[8 + 0][i]; // gather
      
      results[8 + 1][i] = input_data[8 + 1][i]; // gather

      results[8 + 2][i] = input_data[8 + 2][i]; // gather

      results[8 + 3][i] = input_data[8 + 3][i]; // gather

      results[8 + 4][i] = input_data[8 + 4][i]; // gather

      results[8 + 5][i] = input_data[8 + 5][i]; // gather

      results[8 + 6][i] = input_data[8 + 6][i]; // gather

      results[8 + 7][i] = input_data[8 + 7][i]; // gather


      results[0 + 16][i] = input_data[0 + 16][i]; // gather
      
      results[1 + 16][i] = input_data[1 + 16][i]; // gather

      results[2 + 16][i] = input_data[2 + 16][i]; // gather

      results[3 + 16][i] = input_data[3 + 16][i]; // gather

      results[4 + 16][i] = input_data[4 + 16][i]; // gather

      results[5 + 16][i] = input_data[5 + 16][i]; // gather

      results[6 + 16][i] = input_data[6 + 16][i]; // gather

      results[7 + 16][i] = input_data[7 + 16][i]; // gather

      results[8 + 0 + 16][i] = input_data[8 + 0 + 16][i]; // gather
      
      results[8 + 1 + 16][i] = input_data[8 + 1 + 16][i]; // gather

      results[8 + 2 + 16][i] = input_data[8 + 2 + 16][i]; // gather

      results[8 + 3 + 16][i] = input_data[8 + 3 + 16][i]; // gather

      results[8 + 4 + 16][i] = input_data[8 + 4 + 16][i]; // gather

      results[8 + 5 + 16][i] = input_data[8 + 5 + 16][i]; // gather

      results[8 + 6 + 16][i] = input_data[8 + 6 + 16][i]; // gather

      results[8 + 7 + 16][i] = input_data[8 + 7 + 16][i]; // gather


      results[32 + 0][i] = input_data[32 + 0][i]; // gather
      
      results[32 + 1][i] = input_data[32+ 1][i]; // gather

      results[32 + 2][i] = input_data[32+ 2][i]; // gather

      results[32 + 3][i] = input_data[32+ 3][i]; // gather

      results[32 + 4][i] = input_data[32+ 4][i]; // gather

      results[32 + 5][i] = input_data[32+ 5][i]; // gather

      results[32 + 6][i] = input_data[32+ 6][i]; // gather

      results[32 + 7][i] = input_data[32+ 7][i]; // gather

      results[32 + 8 + 0][i] = input_data[32 + 8 + 0][i]; // gather
      
      results[32 + 8 + 1][i] = input_data[32 + 8 + 1][i]; // gather

      results[32 + 8 + 2][i] = input_data[32 + 8 + 2][i]; // gather

      results[32 + 8 + 3][i] = input_data[32 + 8 + 3][i]; // gather

      results[32 + 8 + 4][i] = input_data[32 + 8 + 4][i]; // gather

      results[32 + 8 + 5][i] = input_data[32 + 8 + 5][i]; // gather

      results[32 + 8 + 6][i] = input_data[32 + 8 + 6][i]; // gather

      results[32 + 8 + 7][i] = input_data[32 + 8 + 7][i]; // gather


      results[32 + 0 + 16][i] = input_data[32 + 0 + 16][i]; // gather
      
      results[32 + 1 + 16][i] = input_data[32 + 1 + 16][i]; // gather

      results[32 + 2 + 16][i] = input_data[32 + 2 + 16][i]; // gather

      results[32 + 3 + 16][i] = input_data[32 + 3 + 16][i]; // gather

      results[32 + 4 + 16][i] = input_data[32 + 4 + 16][i]; // gather

      results[32 + 5 + 16][i] = input_data[32 + 5 + 16][i]; // gather

      results[32 + 6 + 16][i] = input_data[32 + 6 + 16][i]; // gather

      results[32 + 7 + 16][i] = input_data[32 + 7 + 16][i]; // gather

      results[32 + 8 + 0 + 16][i] = input_data[32 + 8 + 0 + 16][i]; // gather
      
      results[32 + 8 + 1 + 16][i] = input_data[32 + 8 + 1 + 16][i]; // gather

      results[32 + 8 + 2 + 16][i] = input_data[32 + 8 + 2 + 16][i]; // gather

      results[32 + 8 + 3 + 16][i] = input_data[32 + 8 + 3 + 16][i]; // gather

      results[32 + 8 + 4 + 16][i] = input_data[32 + 8 + 4 + 16][i]; // gather

      results[32 + 8 + 5 + 16][i] = input_data[32 + 8 + 5 + 16][i]; // gather

      results[32 + 8 + 6 + 16][i] = input_data[32 + 8 + 6 + 16][i]; // gather

      results[32 + 8 + 7 + 16][i] = input_data[32 + 8 + 7 + 16][i]; // gather
      
  }
  } else {
    for (size_t arr = 0; arr < num_columns; ++arr) {
      for (size_t i = 0; i < column_size; ++i ) {
	results[arr][i] = input_data[arr][i]; // gather
      }
    }
  }

  
}


/*
void serialstreams
(
 int num_columns,
 size_t column_size,
 const int * __restrict__ * input_data,
 int *__restrict__ * results
)
{
  for (int arr = 0; arr < num_columns, ++arr) {
  for (int i = 0; i < column_size; ++i ) {
      results[arr][i] = input_data[arr][i];
    }
  }
}
*/

void run(benchmark::State & state, int concurrent){
  int ** columns = new int*[state.range_x()] {};
  int ** results = new int*[state.range_x()] {};
  
  for (int i =0; i < state.range_x(); ++i) {
    columns[i] = new int[state.range_y()];
    memset(columns[i], 'x', state.range_y());
    results[i] = new int[state.range_y()];
  }

  char *p = nullptr;
  scanf("%m[a-z]", &p);
  free(p);
  
  while (state.KeepRunning()) {
//     PCM * m = PCM::getInstance();
//     m->resetPMU();
//     auto status = m->program (PCM::DEFAULT_EVENTS, NULL);

//     switch (status)
//       {
//       case PCM::Success:
// 	break;
//       case PCM::MSRAccessDenied:
// 	cerr << "Access to Intel(r) Performance Counter Monitor has denied (no MSR or PCI CFG space access)." << endl;
// 	exit(EXIT_FAILURE);
//       case PCM::PMUBusy:
// 	cerr << "Access to Intel(r) Performance Counter Monitor has denied (Performance Monitoring Unit is occupied by other application). Try to stop the application that use\
// s PMU." << endl;
// 	cerr << "Alternatively you can try running Intel PCM with option -r to reset PMU configuration at your own risk." << endl;
// 	exit(EXIT_FAILURE);
//       default:
// 	cerr << "Access to Intel(r) Performance Counter Monitor has denied (Unknown error)." << endl;
// 	exit(EXIT_FAILURE);
//       }

//     cerr << "\nDetected " << m->getCPUBrandString() << " \"Intel(r) microarchitecture codename " << m->getUArchCodename() << "\"" << endl;
    

//     SystemCounterState before_sstate = getSystemCounterState();
//    state.ResumeTiming();
    streams(state.range_x(), state.range_y(), columns, results, concurrent);
    // state.PauseTiming();

    // SystemCounterState after_sstate = getSystemCounterState();
    // auto ratio = getL3CacheHitRatio(before_sstate, after_sstate);
    // auto hits = getL3CacheHits(before_sstate, after_sstate);
    // auto misses = getL3CacheMisses(before_sstate, after_sstate);

    // printf("hits:  %ul\n", hits);
    // printf("misses:  %ul\n", misses);
    // state.ResumeTiming();
  }

  for (int i =0; i < state.range_x(); ++i) {
    delete[] columns[i];
    delete[] results[i];
  }

  delete[] columns;
  delete[] results;
}

static void BM_concurrent(benchmark::State& state) {
  run(state, 1);
}

static void BM_serial(benchmark::State& state) {
  run(state, 0);
}


// Register the function as a benchmark
BENCHMARK(BM_serial)->RangePair(64, 64, 1<<26, 1<<26);
BENCHMARK(BM_concurrent)->RangePair(64, 64, 1<<26, 1<<26);

BENCHMARK_MAIN();

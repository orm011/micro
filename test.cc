#include <benchmark/benchmark.h>
#include <string.h>
#include <unistd.h>
#include <cpucounters.h>
#include <iostream>
#include <immintrin.h>
#include <iterator>

//_mm256_stream_si256 
using namespace std;
const int k_num_columns = 16;
const int sizes = (1<<27); 

void
streams
(
 size_t column_size,
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ results,
 int concurrent
)
{
  if (concurrent) {
  for (size_t i = 0; i < column_size; ++i ) {
      results[0] ^= input_data[0][i]; // gather
      
      results[1] ^= input_data[1][i]; // gather

      results[2] ^= input_data[2][i]; // gather

      results[3] ^= input_data[3][i]; // gather

      results[4] ^= input_data[4][i]; // gather

      results[5] ^= input_data[5][i]; // gather

      results[6] ^= input_data[6][i]; // gather

      results[7] ^= input_data[7][i]; // gather

      results[8 + 0] ^= input_data[8 + 0][i]; // gather
      
      results[8 + 1] ^= input_data[8 + 1][i]; // gather

      results[8 + 2] ^= input_data[8 + 2][i]; // gather

      results[8 + 3] ^= input_data[8 + 3][i]; // gather

      results[8 + 4] ^= input_data[8 + 4][i]; // gather

      results[8 + 5] ^= input_data[8 + 5][i]; // gather

      results[8 + 6] ^= input_data[8 + 6][i]; // gather

      results[8 + 7] ^= input_data[8 + 7][i]; // gather
  }
  } else {
    for (size_t arr = 0; arr < k_num_columns; ++arr) {
      for (size_t i = 0; i < column_size; ++i ) {
	results[arr] ^= input_data[arr][i]; // gather
      }
    }
  }

  
}

void run(benchmark::State & state, int concurrent){

  const int num_vectors = state.range_x();
  const size_t vector_size_in_ints = state.range_y();
  const int cache_line_size_in_ints = 64/(sizeof(int)); // in ints. assuming 64 bytes in cache line.

  int ** columns = new int*[num_vectors] {};
  int *results = new int[num_vectors] {};

  // allocate enough space for all vectors, as well as some space to spread them out
  size_t total_size_ints = num_vectors * (vector_size_in_ints + cache_line_size_in_ints);
  size_t total_size_bytes = total_size_ints * sizeof(int);
  
  //int * all_cols = (int*)aligned_alloc(64, total_size_bytes);
  //assert(all_cols != nullptr);
  //int * start_offset = all_cols;
  
  for (int i =0; i < num_vectors; ++i) {
    //columns[i] = start_offset;
    columns[i] = new int[vector_size_in_ints];
    memset(columns[i], 'x', vector_size_in_ints*sizeof(int));

    //start_offset += (vector_size_in_ints + cache_line_size_in_ints);
  }

  // char *p = nullptr;
  // size_t size = 0;
  // printf("press enter to continue...\n");
  // auto res = getline(&p, &size, stdin);
  // if (res == -1){
  //   perror("error at getline\n");
  //   exit(1);
  // } else {
  //   printf("continuing...\n");
  // }

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
    streams(state.range_y(), columns, results, concurrent);
    // state.PauseTiming();

    // SystemCounterState after_sstate = getSystemCounterState();
    // auto ratio = getL3CacheHitRatio(before_sstate, after_sstate);
    // auto hits = getL3CacheHits(before_sstate, after_sstate);
    // auto misses = getL3CacheMisses(before_sstate, after_sstate);

    // printf("hits:  %ul\n", hits);
    // printf("misses:  %ul\n", misses);
    // state.ResumeTiming();
  }

  //free(all_cols);
  for (int i = 0; i < num_vectors; ++i) {
    delete[] columns[i];
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
BENCHMARK(BM_serial)->RangePair(k_num_columns, k_num_columns, sizes, sizes);
BENCHMARK(BM_concurrent)->RangePair(k_num_columns, k_num_columns, sizes, sizes);
BENCHMARK_MAIN();

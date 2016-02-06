#include <benchmark/benchmark.h>
#include <string.h>
#include <unistd.h>
//#include <cpucounters.h>
#include <iostream>
#include <immintrin.h>
#include <iterator>
#include <algorithm>

//_mm256_stream_si256 
using namespace std;
const int k_num_columns = 2;
const int k_sizes = (1<<30); 
const int k_check_output = true;

void
gather_interleaved(
 size_t column_size,
 uint32_t*__restrict__ gather_positions,
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output)
{
  for (size_t pos = 0; pos < column_size; ++pos) {
    size_t gpos = gather_positions[pos];
    output[0][pos] = input_data[0][gpos];
    output[1][pos] = input_data[1][gpos];
  }
}

void
gather_series(
 size_t column_size,
 uint32_t *__restrict__ gather_positions, // use 32bit ints as offsets
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output)
{
  for (size_t pos = 0; pos < column_size; ++pos) {
    size_t gpos = gather_positions[pos];
    output[0][pos] = input_data[0][gpos];
  }

  for (size_t pos = 0; pos < column_size; ++pos) {
    size_t gpos = gather_positions[pos];
    output[1][pos] = input_data[1][gpos];
  }
}


void
zip_gather_project(
 size_t column_size,
 uint32_t*__restrict__ gather_positions,
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output)
{
  struct int_pair {int a; int b;};
  int_pair * merged = new int_pair[column_size];
  
  //zip and materialize
  for (size_t pos = 0; pos < column_size; ++pos) {
    merged[pos].a = input_data[0][pos];
    merged[pos].b = input_data[1][pos];
  }

  //gather and project in the same loop
  for (size_t pos = 0; pos < column_size; ++pos) {
    size_t gpos = gather_positions[pos];
    
    output[0][pos] = merged[gpos].a;
    output[1][pos] = merged[gpos].b;
  }
}

void
aggregate_series
(
 size_t column_size,
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ results
)
{
  for (size_t arr = 0; arr < k_num_columns; ++arr) {
    for (size_t i = 0; i < column_size; ++i ) {
      results[arr] ^= input_data[arr][i]; // gather
    }
  }
}

void
aggregate_interleaved
(
 size_t column_size,
 int * __restrict__ * __restrict__ input_data,
 int * __restrict__ results
)
{
  for ( size_t i = 0; i < column_size; ++i ) {
      results[0] ^= input_data[0][i];
      results[1] ^= input_data[1][i];  
      results[2] ^= input_data[2][i];  
      results[3] ^= input_data[3][i];  
      results[4] ^= input_data[4][i];  
      results[5] ^= input_data[5][i];  
      results[6] ^= input_data[6][i];  
      results[7] ^= input_data[7][i];  

      results[8 + 0] ^= input_data[8 + 0][i];  
      results[8 + 1] ^= input_data[8 + 1][i];  
      results[8 + 2] ^= input_data[8 + 2][i];  
      results[8 + 3] ^= input_data[8 + 3][i];  
      results[8 + 4] ^= input_data[8 + 4][i];  
      results[8 + 5] ^= input_data[8 + 5][i];  
      results[8 + 6] ^= input_data[8 + 6][i];  
      results[8 + 7] ^= input_data[8 + 7][i];  
  }
}

template <typename T> void run_full(benchmark::State & state, T func){
  assert(k_sizes == state.range_y());
  assert(k_num_columns == state.range_x());
  
  const int num_vectors = state.range_x();
  const size_t vector_size_in_ints = state.range_y();

  int ** columns = new int*[num_vectors] {};
  int ** outputs = new int*[num_vectors] {};
  uint32_t * positions =  new uint32_t[vector_size_in_ints];

  for (size_t i = 0; i < vector_size_in_ints; ++i) {
    positions[i] = i;
  }

  std::srand(1000);
  std::random_shuffle(positions, positions + vector_size_in_ints);// WARNING: this may pollute perf counters. (but not timing)

  for (int i = 0; i < num_vectors; ++i) {
    columns[i] = new int[vector_size_in_ints];

    for (int j = 0; j < vector_size_in_ints; ++j) {
      columns[i][j] = (i ^ j);
    }
    
    outputs[i] = new int[vector_size_in_ints];
  }

  while (state.KeepRunning()) {
    func(state.range_y(), positions, columns, outputs);
  }

  if (k_check_output) {
    for (int i = 0; i < num_vectors; ++i) {
      for (int j = 0; j < vector_size_in_ints; ++j) {
	int original_j = (outputs[i][j] ^ i);
	assert(positions[j] == original_j);
      }
    }
  }
  
  for (int i = 0; i < num_vectors; ++i) {
    delete[] columns[i];
    delete[] outputs[i];
  }
  
  delete[] columns;
  delete[] outputs;
}

template <typename T> void run(benchmark::State & state, T func) {

  const int num_vectors = state.range_x();
  const size_t vector_size_in_ints = state.range_y();
  //const int cache_line_size_in_ints = 64/(sizeof(int)); // in ints. assuming 64 bytes in cache line.

  int ** columns = new int*[num_vectors] {};
  int *results = new int[num_vectors] {};

  // allocate enough space for all vectors, as well as some space to spread them out
  //size_t total_size_ints = num_vectors * (vector_size_in_ints + cache_line_size_in_ints);
  //size_t total_size_bytes = total_size_ints * sizeof(int);
  
  //int * all_cols = (int*)aligned_alloc(64, total_size_bytes);
  //assert(all_cols != nullptr);
  //int * start_offset = all_cols;
  
  for (int i =0; i < num_vectors; ++i) {
    //columns[i] = start_offset;
    columns[i] = new int[vector_size_in_ints];
    memset(columns[i], 'x', vector_size_in_ints*sizeof(int));
    //start_offset += (vector_size_in_ints + cache_line_size_in_ints);
  }

  while (state.KeepRunning()) {
    func(state.range_y(), columns, results);
  }

  //free(all_cols);
  for (int i = 0; i < num_vectors; ++i) {
    delete[] columns[i];
  }
  
  delete[] columns;
  delete[] results;
}

static void BM_aggregate_interleaved(benchmark::State& state) {
  run(state, aggregate_interleaved);
}

static void BM_aggregate_series(benchmark::State& state) {
  run(state, aggregate_series);
}

static void BM_gather_series(benchmark::State& state) {
  run_full(state, gather_series);
}

static void BM_gather_interleaved(benchmark::State& state) {
  run_full(state, gather_interleaved);
}

static void BM_zip_gather_project(benchmark::State& state) {
  run_full(state, zip_gather_project);
}


// Register the function as a benchmark
BENCHMARK(BM_aggregate_series)->ArgPair(k_num_columns, k_sizes);
BENCHMARK(BM_aggregate_interleaved)->ArgPair(k_num_columns, k_sizes);
BENCHMARK(BM_gather_series)->ArgPair(k_num_columns, k_sizes);
BENCHMARK(BM_gather_interleaved)->ArgPair(k_num_columns, k_sizes);
BENCHMARK(BM_zip_gather_project)->ArgPair(k_num_columns, k_sizes);

BENCHMARK_MAIN();

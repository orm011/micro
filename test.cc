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
const int k_fact_table_size = (1 << 30);  // 1 billion entries. => 4GB
const int k_dimension_table_size = (1 << 25); 
const int k_num_columns = 3;
const int k_check_output = true;

uint32_t * gather_positions = nullptr;
uint64_t checksum = 0;

void
gather_interleaved(
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output)
{
  for (size_t pos = 0; pos < k_fact_table_size; ++pos) {
    size_t gpos = gather_positions[pos];
    output[0][pos] = input_data[0][gpos];
    output[1][pos] = input_data[1][gpos];
    output[2][pos] = input_data[2][gpos];
  }
}

void
gather_series(
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output)
{
  for (size_t pos = 0; pos < k_fact_table_size; ++pos) {
    size_t gpos = gather_positions[pos];
    output[0][pos] = input_data[0][gpos];
  }

  for (size_t pos = 0; pos < k_fact_table_size; ++pos) {
    size_t gpos = gather_positions[pos];
    output[1][pos] = input_data[1][gpos];
  }

  for (size_t pos = 0; pos < k_fact_table_size; ++pos) {
    size_t gpos = gather_positions[pos];
    output[2][pos] = input_data[2][gpos];
  }
}


void
zip_gather_project(
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output)
{
  struct int_pair {int mem[k_num_columns]; };
  int_pair * merged = new int_pair[k_fact_table_size];
  
  //zip and materialize
  for (size_t pos = 0; pos < k_dimension_table_size; ++pos) {
    merged[pos].mem[0] = input_data[0][pos];
    merged[pos].mem[1] = input_data[1][pos];
    merged[pos].mem[2] = input_data[2][pos];
  }

  //gather and project in the same loop
  for (size_t pos = 0; pos < k_fact_table_size; ++pos) {
    size_t gpos = gather_positions[pos];
    
    output[0][pos] = merged[gpos].mem[0];
    output[1][pos] = merged[gpos].mem[1];
    output[2][pos] = merged[gpos].mem[2];
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

uint64_t get_checksum(uint32_t *array, size_t len){
  uint64_t acc = 0;
  for (size_t i = 0; i < k_fact_table_size; ++i){
    acc = (acc * 7) + gather_positions[i];
  }
  return acc;
}


template <typename T> void run_full(benchmark::State & state, T func){
  int ** columns = new int*[k_num_columns] {};
  int ** outputs = new int*[k_num_columns] {};

  if (!gather_positions) {
    gather_positions =  new uint32_t[k_fact_table_size];

    for (size_t i = 0; i < k_fact_table_size; ++i) {
      gather_positions[i] = i % (k_dimension_table_size);
    }

    std::srand(1000);
    std::random_shuffle(gather_positions, gather_positions + k_fact_table_size);
    checksum = get_checksum(gather_positions, k_fact_table_size);
  }

  for (int i = 0; i < k_num_columns; ++i) {
    columns[i] = new int[k_dimension_table_size];

    for (int j = 0; j < k_dimension_table_size; ++j) {
      columns[i][j] = (i ^ j);
    }
    
    outputs[i] = new int[k_fact_table_size];
  }

  while (state.KeepRunning()) {
    func(columns, outputs);
  }

  

  int64_t newck = get_checksum(gather_positions, k_fact_table_size);
  assert(checksum == newck);

  if (k_check_output) {
    for (int i = 0; i < k_num_columns; ++i) {
      for (int j = 0; j < k_fact_table_size; ++j) {
	int original_j = (outputs[i][j] ^ i);
	assert(gather_positions[j] == original_j);
      }
    }
  }
  
  for (int i = 0; i < k_num_columns; ++i) {
    delete[] columns[i];
    delete[] outputs[i];
  }
  
  delete[] columns;
  delete[] outputs;
}

template <typename T> void run(benchmark::State & state, T func) {

  //const int cache_line_size_in_ints = 64/(sizeof(int)); // in ints. assuming 64 bytes in cache line.

  int ** columns = new int*[k_num_columns] {};
  int *results = new int[k_num_columns] {};

  // allocate enough space for all vectors, as well as some space to spread them out
  //size_t total_size_ints = k_num_columns * (k_fact_table_size + cache_line_size_in_ints);
  //size_t total_size_bytes = total_size_ints * sizeof(int);
  
  //int * all_cols = (int*)aligned_alloc(64, total_size_bytes);
  //assert(all_cols != nullptr);
  //int * start_offset = all_cols;
  
  for (int i =0; i < k_num_columns; ++i) {
    //columns[i] = start_offset;
    columns[i] = new int[k_fact_table_size];
    memset(columns[i], 'x', k_fact_table_size*sizeof(int));
    //start_offset += (k_fact_table_size + cache_line_size_in_ints);
  }

  while (state.KeepRunning()) {
    func(state.range_y(), columns, results);
  }

  //free(all_cols);
  for (int i = 0; i < k_num_columns; ++i) {
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
BENCHMARK(BM_aggregate_series);
BENCHMARK(BM_aggregate_interleaved);
BENCHMARK(BM_gather_series);
BENCHMARK(BM_gather_interleaved);
BENCHMARK(BM_zip_gather_project);

BENCHMARK_MAIN();

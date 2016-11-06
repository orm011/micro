#include <benchmark/benchmark.h>
#include <string.h>
#include <unistd.h>
//#include <cpucounters.h>
#include <iostream>
#include <immintrin.h>
#include <iterator>
#include <algorithm>
#include <tbb/tbb.h>

//_mm256_stream_si256 
using namespace std;
const int k_fact_table_size = (1 << 28); 
const int k_dimension_table_size = (1 << 20); 
const int k_num_columns = 2;
const int k_check_output = true;

const uint32_t * gather_positions = nullptr;
uint64_t checksum = 0;

void
gather_interleaved(
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output,
 size_t output_size)
{
	using namespace tbb;
	parallel_for(blocked_range<size_t>(0, output_size, (1<<10)),
							 [&](const auto & range){
								 for (size_t pos = range.begin(); pos < range.end(); ++pos) {
										 size_t gpos = gather_positions[pos];
										 output[0][pos] = input_data[0][gpos]; // gather first column
										 output[1][pos] = input_data[1][gpos]; // gather second column
										 // output[2][pos] = input_data[2][gpos]; // gather third column
										 // output[3][pos] = input_data[3][gpos]; // gather fourth column
									 }
							 });
}

void
gather_series(
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output,
 size_t output_size)
{
	using namespace tbb;
	parallel_for(blocked_range<size_t>(0, output_size, (1<<10)),
							 [&](const auto & range){
								 for (size_t pos = range.begin(); pos < range.end(); ++pos) {
									 size_t gpos = gather_positions[pos];
									 output[0][pos] = input_data[0][gpos];
								 }

								 // for (size_t pos = range.begin(); pos < range.end(); ++pos) {
								 // 	 size_t gpos = gather_positions[pos];
								 // 	 output[2][pos] = input_data[2][gpos];
								 // }


								 // for (size_t pos = range.begin(); pos < range.end(); ++pos) {
								 // 	 size_t gpos = gather_positions[pos];
								 // 	 output[3][pos] = input_data[3][gpos];
								 // }

							 });


	parallel_for(blocked_range<size_t>(0, output_size, (1<<10)),
							 [&](const auto & range){
								 for (size_t pos = range.begin(); pos < range.end(); ++pos) {
									 size_t gpos = gather_positions[pos];
									 output[1][pos] = input_data[1][gpos];
								 }});

}


void
zip_gather_project(
 int *__restrict__ * __restrict__ input_data,
 int *__restrict__ * __restrict__ output,
 size_t output_size)
{
  struct int_pair {int mem[k_num_columns]; };
  int_pair * dimension_rows = new int_pair[k_dimension_table_size];

	using namespace tbb;
	size_t grain_size = 1<<15;
	parallel_for(blocked_range<size_t>(0, k_dimension_table_size, grain_size),
							 [&](const auto &range){
								 // zip
								 for (size_t offset = range.begin(); offset != range.end(); ++offset) {
									 dimension_rows[offset].mem[0] = input_data[0][offset];
									 dimension_rows[offset].mem[1] = input_data[1][offset];
									 // dimension_rows[offset].mem[2] = input_data[2][offset];
									 // dimension_rows[offset].mem[3] = input_data[3][offset];
								 }
							 });


	parallel_for(blocked_range<size_t>(0, output_size, grain_size),
							 [&](const auto & range){
								 // gather and project in the same loo
								 for (size_t offset = range.begin(); offset != range.end(); ++offset) {
									 size_t gpos = gather_positions[offset];
									 output[0][offset] = dimension_rows[gpos].mem[0];
									 output[1][offset] = dimension_rows[gpos].mem[1];
									 // output[2][offset] = dimension_rows[gpos].mem[2];
									 // output[3][offset] = dimension_rows[gpos].mem[3];
								 }
							 });
}


uint64_t get_checksum(const uint32_t *array, size_t len){
  uint64_t acc = 0;
  for (size_t i = 0; i < k_fact_table_size; ++i){
    acc = (acc * 7) + array[i];
  }
  return acc;
}


template <typename T> void run_full(benchmark::State & state, T func){
  int ** columns = new int*[k_num_columns] {};
  int ** outputs = new int*[k_num_columns] {};

  if (!gather_positions) {
    auto tmp =  new uint32_t[k_fact_table_size];

    for (size_t i = 0; i < k_fact_table_size; ++i) {
      tmp[i] = i % (k_dimension_table_size);
    }

    std::srand(1000);
    std::random_shuffle(tmp, tmp + k_fact_table_size);
    checksum = get_checksum(tmp, k_fact_table_size);
		gather_positions = tmp;
  }

  for (int i = 0; i < k_num_columns; ++i) {
    columns[i] = new int[k_dimension_table_size];

    for (int j = 0; j < k_dimension_table_size; ++j) {
      columns[i][j] = (i ^ j);
    }
    
    outputs[i] = new int[k_fact_table_size];
  }

	while (state.KeepRunning()) {
			func(columns, outputs, k_fact_table_size);
	}

  int64_t newck = get_checksum(gather_positions, k_fact_table_size);
  assert(checksum == newck);

  if (k_check_output) {
		cout << "checking output" << endl;
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
BENCHMARK(BM_gather_series);
BENCHMARK(BM_gather_interleaved);
BENCHMARK(BM_zip_gather_project);

BENCHMARK_MAIN();

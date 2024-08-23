/**
 * There're two key operation that could yield unexpected results.
 * 1. XD3_ALLOCSIZE definition in xdelta.h
 * 		if we use CDC instead of fixed-size chunking, the XD3_ALLOCSIZE 
 *      should be 4 * Average chunk size
 * 
 * 2. The last parameter of func LZ4_compress_default(). It shoule be the 
 *     a. max-chunk-size or b. `f.trace[i].size`. Or it will return 0, which
 *     is regared as the minimum case to be stored.
 * 
 */
#include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <cmath>
#include <chrono>
#include <algorithm>
#include "odess.h"
#include "../compress_cdc.h"
#include "../lz4.h"
#include "../xxhash.h"
#include "../xdelta3/xdelta3.h"
#define INF 987654321
using namespace std;



int main(int argc, char* argv[]) {
	if (argc != 6) {
		cerr << "usage: ./lsh_inf [input_file] [window_size] [SF_NUM] [FEATURE_NUM] [average_chunk_size]\n";
		exit(0);
	}
	int W = atoi(argv[2]);
	int SF_NUM = atoi(argv[3]);
	int FEATURE_NUM = atoi(argv[4]);

	int AVG_CHUNK_SIZE = atoi(argv[5]);

	DATA_IO f(argv[1]);
	f.read_file(AVG_CHUNK_SIZE);

	map<XXH64_hash_t, int> dedup;
	Odess lsh(BLOCK_SIZE, W, SF_NUM, FEATURE_NUM); // parameter

	unsigned long long total = 0;
	unsigned long long total_non_duplicated = 0;
	int delta_chunks = 0;
	int no_delta_chunks = 0;
	int error_chunks = 0;

	char compressed[4 * AVG_CHUNK_SIZE];
	char delta_compressed[4 * AVG_CHUNK_SIZE];
	long long time_cost = 0;
	// f.time_check_start();
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < f.N; ++i) {
		XXH64_hash_t h = XXH64(f.trace[i].data, f.trace[i].size, 0);

		if (dedup.count(h)) { // deduplication
			continue;
		}

		int dcomp_lsh_ref = lsh.request((unsigned char*)f.trace[i].data, f.trace[i].size);
	}
	auto finish = std::chrono::high_resolution_clock::now();

    // 计算持续时间
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
	cout <<"******************** ODES - Begin ********************\n";
	printf("Trace: %s\n", argv[1]);
	cout <<"Total time: " << elapsed << "us\n";
	cout <<"********************* ODES - End *********************\n";
}

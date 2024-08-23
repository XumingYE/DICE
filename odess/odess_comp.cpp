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
	for (int i = 0; i < f.N; ++i) {
		XXH64_hash_t h = XXH64(f.trace[i].data, f.trace[i].size, 0);

		if (dedup.count(h)) { // deduplication
			continue;
		}

		total_non_duplicated += f.trace[i].size;
		
		dedup[h] = i;
		int dcomp_lsh = INF, dcomp_lsh_ref;

		f.time_check_start();
		dcomp_lsh_ref = lsh.request((unsigned char*)f.trace[i].data, f.trace[i].size);
		time_cost += f.time_check_end();

		if (dcomp_lsh_ref != -1) {
			dcomp_lsh = xdelta3_compress((char *)f.trace[i].data, f.trace[i].size, (char *)f.trace[dcomp_lsh_ref].data, f.trace[dcomp_lsh_ref].size, delta_compressed, 1);
			if (dcomp_lsh > f.trace[i].size) 
				error_chunks++;
		}

		if (f.trace[i].size > dcomp_lsh) { // delta compress
			total += dcomp_lsh;
			delta_chunks ++;
		}
		else {
			total += f.trace[i].size;
			no_delta_chunks ++;
			lsh.insert(i);
		}
		
	}
	cout <<"******************** ODES - Begin ********************\n";
	printf("Trace: %s\n", argv[1]);
	cout <<"Avg chunk size: " << AVG_CHUNK_SIZE << "us\n";
	cout <<"Total time: " << time_cost << "us\n";
	printf("LSH: Odess, W = %d, SF = %d, feature = %d\n", W, SF_NUM, FEATURE_NUM);
	printf("Total      Chunks: %d\n", f.N);
	printf("Duplicated Chunks: %d\n", f.N - delta_chunks - no_delta_chunks);
	printf("Base       Chunks: %d\n", no_delta_chunks);
	printf("Similar    Chunks: %d\n", delta_chunks);
	printf("Error Chunks: %d, DCE=%.5lf\n", error_chunks, 1.0 * error_chunks / (delta_chunks + no_delta_chunks));
	printf("Non-Dedup size: %llu\n", total_non_duplicated);
	printf("File size: %llu (%.2lf%%)\n", f.size, 1.0 * f.size / total);
	printf("Final  size: %llu (%.2lf%%)\n", total, 1.0 * total_non_duplicated / total);
	cout <<"********************* ODES - End *********************\n";
}

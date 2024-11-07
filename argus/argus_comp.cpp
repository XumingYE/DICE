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
#include "argus.h"
#include "../compress_cdc.h"
#include "../lz4.h"
#include "../xxhash.h"
#include "../xdelta3/xdelta3.h"
#define INF 987654321
using namespace std;



int main(int argc, char* argv[]) {
	if (argc != 5) {
		cerr << "usage: ./lsh_inf [input_file] [byte_shift] [FEATURE_NUM] [average_chunk_size]\n";
		exit(0);
	}
	int byte_shift = atoi(argv[2]);
	int FEATURE_NUM = atoi(argv[3]);

	int AVG_CHUNK_SIZE = atoi(argv[4]);

	DATA_IO f(argv[1]);
	f.read_file(AVG_CHUNK_SIZE);

	map<XXH64_hash_t, int> dedup;
	Argus lsh(BLOCK_SIZE, byte_shift, FEATURE_NUM); // parameter

	unsigned long long total = 0;
	unsigned long long total_non_duplicated = 0;
	int delta_chunks = 0;
	int no_delta_chunks = 0;
	int error_chunks = 0;

	unsigned char delta_compressed[32 * AVG_CHUNK_SIZE];
	long long time_cost = 0;

	/* Reset Xdelta3 with no secondary compression*/
	xd3_config config;
    memset(&config, 0, sizeof(config));

    // 设置配置参数，禁用内置压缩和二次压缩
    config.flags |= XD3_NOCOMPRESS;   // 禁用内置的普通压缩算法
    config.flags |= XD3_SEC_NOALL;    // 禁用二次压缩的所有部分

    // 确保未启用任何二次压缩算法
    config.flags &= ~(XD3_SEC_DJW | XD3_SEC_FGK | XD3_SEC_LZMA);
	// f.time_check_start();
	for (int i = 0; i < f.N; ++i) {
		XXH64_hash_t h = XXH64(f.trace[i].data, f.trace[i].size, 0);

		if (dedup.count(h)) { // deduplication
			continue;
		}

		total_non_duplicated += f.trace[i].size;
		
		dedup[h] = i;
		int dcomp_lsh_ref;
		usize_t dcomp_lsh = INF;

		f.time_check_start();
		dcomp_lsh_ref = lsh.request((unsigned char*)f.trace[i].data, f.trace[i].size);
		time_cost += f.time_check_end();

		if (dcomp_lsh_ref != -1) {
			int ret = xd3_encode_memory(f.trace[i].data, f.trace[i].size, f.trace[dcomp_lsh_ref].data, f.trace[dcomp_lsh_ref].size, delta_compressed, &dcomp_lsh, sizeof(delta_compressed), config.flags);

			if (ret != 0){
				fprintf(stderr, "xdelta3 encode error! ret=%d\tallocate_space=%lu\n", ret, sizeof(delta_compressed));
			}
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
	cout <<"******************** Argus - Begin ********************\n";
	printf("Trace: %s\n", argv[1]);
	cout <<"Avg chunk size: " << AVG_CHUNK_SIZE << "us\n";
	cout <<"Total time: " << time_cost << "us\n";
	printf("LSH: Argus, byte_shift = %d, feature = %d\n", byte_shift, FEATURE_NUM);
	printf("Total      Chunks: %d\n", f.N);
	printf("Duplicated Chunks: %d\n", f.N - delta_chunks - no_delta_chunks);
	printf("Base       Chunks: %d\n", no_delta_chunks);
	printf("Similar    Chunks: %d\n", delta_chunks);
	printf("Error Chunks: %d, DCE=%.5lf\n", error_chunks, 1.0 * error_chunks / (delta_chunks + no_delta_chunks));
	printf("Non-Dedup size: %llu\n", total_non_duplicated);
	printf("File size: %llu (%.2lf%%)\n", f.size, 1.0 * f.size / total);
	printf("Final  size: %llu (%.2lf%%)\n", total, 1.0 * total_non_duplicated / total);
	cout <<"********************* Argus - End *********************\n";
}

#include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <cmath>
#include <algorithm>
#include "dicev3.h"
#include "../compress_cdc.h"
#include "../lz4.h"
#include "../xxhash.h"
// extern "C" {
// 	#include "../xdelta3/xdelta3.h"
// }
#include "../xdelta3/xdelta3.h"
#define INF 987654321
using namespace std;

int print_data(const uint8_t *data, size_t size) {
    printf("Data (first 20 bytes): ");
    for (size_t i = 0; i < size && i < 20; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {

	if (argc != 6) {
		cerr << "usage: ./lsh_inf [input_file] [average_chunk_size] [dimension] [buffer_size] [threshold]\n";
		exit(0);
	}

	vector<int> byte_slice = {2, 3};
	
	int AVG_CHUNK_SIZE = atoi(argv[2]);
	DATA_IO f(argv[1]);
	f.read_file(AVG_CHUNK_SIZE);

	DICE dice = DICE(atoi(argv[3]), atoi(argv[4]), atof(argv[5]), int(f.N * 0.8));

	map<XXH64_hash_t, int> dedup;

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
    // config.flags |= XD3_SEC_NOALL;    // 禁用二次压缩的所有部分
	config.flags |= XD3_COMPLEVEL_9;

    // 确保未启用任何二次压缩算法
    config.flags &= ~(XD3_SEC_DJW | XD3_SEC_FGK | XD3_SEC_LZMA);

	// f.time_check_start();
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < f.N; ++i) {
		XXH64_hash_t h = XXH64(f.trace[i].data, f.trace[i].size, 0);

		if (dedup.count(h)) { // deduplication
			continue;
		}
		dedup[h] = i;
		total_non_duplicated += f.trace[i].size;

		int dcomp_ann_ref;
		usize_t dcomp_ann = INF;

		f.time_check_start();
		dcomp_ann_ref = dice.request(f.trace[i].data, f.trace[i].size);
		time_cost += f.time_check_end();
		int output_size = -1;
		
		if (dcomp_ann_ref != -1) {
			
			int ret = xd3_encode_memory(f.trace[i].data, f.trace[i].size, f.trace[dcomp_ann_ref].data, f.trace[dcomp_ann_ref].size, delta_compressed, &dcomp_ann, sizeof(delta_compressed), config.flags);

			if (ret != 0){
				fprintf(stderr, "xdelta3 encode error! ret=%d\tallocate_space=%lu\n", ret, sizeof(delta_compressed));
				fprintf(stderr, "Input Data: \n");
				print_data(f.trace[i].data, f.trace[i].size);
				fprintf(stderr, "Source Data: \n");
				print_data(f.trace[dcomp_ann_ref].data, f.trace[dcomp_ann_ref].size);
				fprintf(stderr, "Delta Compressed Size: %lu\n", sizeof(delta_compressed));
				fprintf(stderr, "Compression Level: %d\n", xd3_flags::XD3_COMPLEVEL_9);
				// fprintf(stderr, "xdelta3 encode error! ret=%d\tallocate_space=%lu", ret, sizeof(delta_compressed));
				// return -1;  // 或其它错误处理
			}

			// dcomp_ann = xdelta3_compress((char *)f.trace[i].data, f.trace[i].size, (char *)f.trace[dcomp_ann_ref].data, f.trace[dcomp_ann_ref].size, delta_compressed, 1);

			if (dcomp_ann > f.trace[i].size) 
				error_chunks++;
		}

		if (f.trace[i].size > dcomp_ann) { // delta compress
			total += dcomp_ann;
			delta_chunks ++;
		}
		else {
			total += f.trace[i].size;
			no_delta_chunks ++;
			dice.insert(i);
		}

	}
	auto finish = std::chrono::high_resolution_clock::now();

    // 计算持续时间
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
	cout <<"******************** DICE - Begin ********************\n";
	printf("Trace: %s\n", argv[1]);
	cout <<"Total time: " << elapsed << "us\n";
	cout <<"Avg chunk size: " << AVG_CHUNK_SIZE << "us\n";
	cout <<"Request time: " << time_cost << "us\n";
	
	printf("Total      Chunks: %d\n", f.N);
	printf("Duplicated Chunks: %d\n", f.N - delta_chunks - no_delta_chunks);
	printf("Base       Chunks: %d\n", no_delta_chunks);
	printf("Similar    Chunks: %d\n", delta_chunks);
	printf("Error Chunks: %d, DCE=%.5lf\n", error_chunks, 1.0 * error_chunks / (delta_chunks + no_delta_chunks));
	printf("Non-Dedup size: %llu\n", total_non_duplicated);
	printf("File size: %llu (%.2lf%%)\n", f.size, 1.0 * f.size / total);
	printf("Final  size: %llu (%.2lf%%)\n", total, 1.0 * total_non_duplicated / total);
	cout <<"********************* DICE - End *********************\n";
    return 0;

}

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


int main(int argc, char* argv[]) {

	if (argc != 6) {
		cerr << "usage: ./lsh_inf [input_file] [average_chunk_size] [dimension] [buffer_size] [threshold]\n";
		exit(0);
	}

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
		int dcomp_ann_ref = dice.request(f.trace[i].data, f.trace[i].size);
	}
	auto finish = std::chrono::high_resolution_clock::now();

    // 计算持续时间
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
	cout <<"******************** DICE - Begin ********************\n";
	printf("Trace: %s\n", argv[1]);
	cout <<"Total time: " << elapsed << "us\n";
	cout <<"********************* DICE - End *********************\n";
    return 0;

}

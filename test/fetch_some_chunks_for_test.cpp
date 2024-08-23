#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <bitset>
#include <map>
#include <cmath>
#include <algorithm>
#include <random>
#include "dicev2.h"
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

	if (argc != 3) {
		cerr << "usage: ./lsh_inf [input_file] [average_chunk_size] \n";
		exit(0);
	}

	vector<int> byte_slice = {2, 3};
	DICE dice = DICE(1, 128, 0.216778, byte_slice);

	int AVG_CHUNK_SIZE = atoi(argv[2]);
	DATA_IO f(argv[1]);
	f.read_file(AVG_CHUNK_SIZE);

	map<XXH64_hash_t, int> dedup;

	unsigned long long total = 0;
	f.time_check_start();

    // 随机数生成器
    std::random_device rd;  // 获得随机种子
    std::mt19937 gen(rd()); // 生成随机数

    // 定义随机数分布范围
    int min = 0;
    int max = f.N;
    std::uniform_int_distribution<> dist(min, max);

    char compressed[4 * AVG_CHUNK_SIZE];
    char delta_compressed[4 * AVG_CHUNK_SIZE];

    // 生成多个随机整数
    int num_values = 10; // 要生成的随机值数量
    std::vector<int> random_values(num_values);

    for (int& value : random_values) {
        value = dist(gen);
    }

    // 输出生成的随机整数
    std::cout << "Random numbers between " << min << " and " << max << ":" << std::endl;
    for (const int i : random_values) {
        // 生成文件名
        std::ostringstream filename;
        filename << "chunk-" << i << ".bin";

        // 将 chunk 保存为文件
        std::ofstream outfile(filename.str(), std::ios::binary);
        if (outfile.is_open()) {
            outfile.write((char*)f.trace[i].data, f.trace[i].size);
            outfile.close();
        } else {
            std::cerr << "Failed to open file " << filename.str() << " for writing.\n";
        }
        for (int j = i + 1; j <= i + 10; ++j) {
            int dcomp_ann = xdelta3_compress((char *)f.trace[i].data, f.trace[i].size, (char *)f.trace[j].data, f.trace[j].size, delta_compressed, 1);
            std::cout << "chunk-" << i << " with chunk-" << i+j << ". Ratio = (" << f.trace[i].size << "/" << dcomp_ann << ")\n";

            std::ostringstream filename;
            filename << "chunk-" << j << ".bin";

            // 将 chunk 保存为文件
            std::ofstream outfile(filename.str(), std::ios::binary);
            if (outfile.is_open()) {
                outfile.write((char*)f.trace[j].data, f.trace[j].size);
                outfile.close();
            } else {
                std::cerr << "Failed to open file " << filename.str() << " for writing.\n";
            }
        }
        std::cout << "====================================\n";
    }

	cout << "Total time: " << f.time_check_end() << "us\n";
	printf("Trace: %s\n", argv[1]);
	printf("Origin size: %ld\n", f.size);
	printf("Final  size: %llu (%.2lf%%)\n", total, (double)total * 100 / f.size);
    return 0;

}

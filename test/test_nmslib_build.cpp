/**
 * 该测试说明annoy如果在build之后添加元素，需要在unbuild之后再build一次，否则后面添加的元素不会被索引
 * 推荐在添加元素之前unbuild，其实都无所谓
 */

#include <tuple>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <omp.h>
#include <functional>
#include <random>
#include <bitset>
#include <chrono>
#include <armadillo>
#include "../compress_cdc.h"
#include "../lz4.h"
#include "../xxhash.h"
#include "../hnswlib/hnswlib.h"
// extern "C" {
// 	#include "../xdelta3/xdelta3.h"
// }
// #include "../xdelta3/xdelta3.h"
#define INF 987654321
// using namespace std;

#define DIMENSION 128
#define THREADS 4
// #define ANNOYLIB_MULTITHREADED_BUILD 1
typedef float ANNOYTYPE;

inline double euclidean_distance(const arma::frowvec& vec1, const arma::frowvec& vec2) {
    return arma::norm(vec1 - vec2, 2); // 计算L2范数（欧几里得距离）
}


int main(int argc, char* argv[]) {
	int f = 128;
	int max_elements = 10000;   // Maximum number of elements, should be known beforehand
    int M = DIMENSION;                 // Tightly connected with internal dimensionality of the data
                                // strongly affects the memory consumption
    int ef_construction = 200;  // Controls index search speed/build speed tradeoff
	std::chrono::high_resolution_clock::time_point t_start, t_end;


    // Initing index
    hnswlib::L2Space space(DIMENSION);
    hnswlib::HierarchicalNSW<float>* alg_hnsw = new hnswlib::HierarchicalNSW<float>(&space, max_elements, M, ef_construction);
	std::vector<std::pair<int, arma::frowvec>> linear;

	std::cout << "Building index ... be patient !!" << std::endl;
	std::cout << "\"Trees that are slow to grow bear the best fruit\" (Moliere)" << std::endl;


	arma::frowvec vec;
	for(int i=0; i<10000; ++i){
		vec = arma::randu<arma::frowvec>(DIMENSION);
		linear.emplace_back(i, vec);
	}

	bool built = false;

	t_start = std::chrono::high_resolution_clock::now();
	for (const auto& pair : linear) {
		// std::cout << "Push\n";
		// std::cout << pair.first << " ";
		// std::cout << "\n";
		alg_hnsw->addPoint(pair.second.memptr(), pair.first);
	
	}
	t_end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t_end - t_start ).count();
	std::cout << " Done in "<< duration << " us." << std::endl;

	vec = arma::randu<arma::frowvec>(DIMENSION);

	double dist_linear = 999.9;
	int ret_linear = -1;
	for (int i = linear.size() - 1; i >= 0; --i) {
		double nowdist = euclidean_distance(linear[i].second, vec);
		// std::cout << "Linear Index: " << linear[i].first << ", Distance: " << nowdist << "\n vector = "<< linear[i].second << std::endl;
		if (dist_linear > nowdist) {
			dist_linear = nowdist;
			ret_linear = linear[i].first;
		}
	}
	std::cout << "Linear Index: " << ret_linear << ", Distance: " << dist_linear << "\n vector = "<< std::endl;

	std::priority_queue<std::pair<float, size_t>> result = alg_hnsw->searchKnn(vec.memptr(), 1);

	// 输出结果
	// std::cout << "Nearest neighbors:" << std::endl;
	std::cout << "Index: " << result.top().second << ", Distance: " << result.top().first << std::endl;

    return 0;

}

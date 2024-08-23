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
#include "../annoy/src/annoylib.h"
#include "../annoy/src/kissrandom.h"
// extern "C" {
// 	#include "../xdelta3/xdelta3.h"
// }
// #include "../xdelta3/xdelta3.h"
#define INF 987654321
// using namespace std;

#define DIMENSION 128
#define THREADS 4
using namespace Annoy;
// #define ANNOYLIB_MULTITHREADED_BUILD 1
typedef float ANNOYTYPE;
typedef Annoy::AnnoyIndex<int, ANNOYTYPE, Annoy::Euclidean, Annoy::Kiss64Random, Annoy::AnnoyIndexSingleThreadedBuildPolicy> MyAnnoyIndex;

inline double euclidean_distance(const arma::frowvec& vec1, const arma::frowvec& vec2) {
    return arma::norm(vec1 - vec2, 2); // 计算L2范数（欧几里得距离）
}


int main(int argc, char* argv[]) {
	int f = 128;
	std::chrono::high_resolution_clock::time_point t_start, t_end;
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(-1.0, 1.0);

	//******************************************************
	//Building the tree
	std::vector<std::pair<int, arma::frowvec>> linear;
	AnnoyIndex<int, float, Euclidean, Kiss32Random, AnnoyIndexMultiThreadedBuildPolicy> t = AnnoyIndex<int, float, Euclidean, Kiss32Random, AnnoyIndexMultiThreadedBuildPolicy>(f);

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
		t.add_item(pair.first, pair.second.memptr());

		if (!built && pair.first > 128){
			
			t.build(2 * DIMENSION);
			
			built = true;
		}
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

	std::vector<int> closest;
	std::vector<ANNOYTYPE> distance;

	// vector<ANNOYTYPE> tmp(DIMENSION);
	// t.get_item(2, tmp.data());
	// std::cout << "2 = ";
	// for (auto &item : tmp) {
	// 	std::cout << item << " ";
	// }
	// std::cout << "\n";
	std::cout << t.get_n_items() << "\n";
	t.add_item(10001, vec.memptr());
	std::cout << t.get_n_items() << "\n";

	t.add_item(10012, vec.memptr());
	t.get_nns_by_vector(vec.memptr(), 1, -1, &closest, &distance);

	std::cout << "Index: " << closest[0] << ", Distance: " << distance[0] << std::endl;
	std::cout << "closest size: " << closest.size() << std::endl;

    return 0;

}

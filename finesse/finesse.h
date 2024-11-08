/**
 * 
 * 2024.10.28 修改，使用gear hash + limited hash width
 */
#include <tuple>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <random>
#include "../xxhash.h"
#include "../fastcdc/fastcdc.h"

// finesse
typedef uint32_t feature_t;
typedef uint32_t superfeature_t;



class Finesse {
	private:
	std::mt19937 gen1, gen2;
	std::uniform_int_distribution<uint32_t> full_uint32_t;

	int BLOCK_SIZE, window;
	int SF_NUM, FEATURE_NUM;

	int* subchunkIndex;

	const feature_t A = 37, MOD = 1000000007;
	feature_t Apower = 1;

	feature_t* feature;
	superfeature_t* superfeature;

	std::map<superfeature_t, std::vector<int>>* sfTable;
	// std::vector< std::pair< std::vector<uint64_t>, int> > sfTable;
	public:
	Finesse(int _BLOCK_SIZE, int _W, int _SF_NUM, int _FEATURE_NUM) {
		gen1 = std::mt19937(922);
		gen2 = std::mt19937(314159);
		full_uint32_t = std::uniform_int_distribution<uint32_t>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());

		BLOCK_SIZE = _BLOCK_SIZE;
		window = _W;
		SF_NUM = _SF_NUM;
		FEATURE_NUM = _FEATURE_NUM;

		feature = new feature_t[FEATURE_NUM];
		superfeature = new superfeature_t[SF_NUM];
		subchunkIndex = new int[FEATURE_NUM + 1];
		subchunkIndex[0] = 0;
		
		sfTable = new std::map<superfeature_t, std::vector<int>>[SF_NUM];

		for (int i = 0; i < window - 1; ++i) {
			Apower *= A;
			Apower %= MOD;
		}
	}
	~Finesse() {
		delete[] feature;
		delete[] superfeature;
		delete[] subchunkIndex;
		delete[] sfTable;
	}
	int request(unsigned char* ptr, int size);
	void insert(int label);
};

int Finesse::request(unsigned char* ptr, int size) {
	for (int i = 0; i < FEATURE_NUM; ++i) feature[i] = 0;
	for (int i = 0; i < SF_NUM; ++i) superfeature[i] = 0;
	for (int i = 0; i < FEATURE_NUM; ++i) {
		subchunkIndex[i + 1] = (size * (i + 1)) / FEATURE_NUM;
	}

	// 下面是原版的finesse实现
	// for (int i = 0; i < FEATURE_NUM; ++i) {
	// 	feature_t fp = 0;

	// 	for (int j = subchunkIndex[i]; j < subchunkIndex[i] + window; ++j) {
	// 		fp *= A;
	// 		fp += (unsigned char)ptr[j];
	// 		fp %= MOD;
	// 	}

	// 	for (int j = subchunkIndex[i]; j < subchunkIndex[i + 1] - window + 1; ++j) {
	// 		if (fp > feature[i]) feature[i] = fp;

	// 		fp -= (ptr[j] * Apower) % MOD;
	// 		while (fp < 0) fp += MOD;
	// 		if (j != subchunkIndex[i + 1] - window) {
	// 			fp *= A;
	// 			fp += ptr[j + window];
	// 			fp %= MOD;
	// 		}
	// 	}
	// }

	// 下面是gear hash的实现
	int byte_shift = 2;
	for (int i = 0; i < FEATURE_NUM; ++i) {
		for (int j = subchunkIndex[i]; j < subchunkIndex[i + 1]; ++j) {
			uint64_t fp = 0;
			fp = (fp << byte_shift) + (GEARv2[ptr[j]]);
			if (fp > feature[i]) feature[i] = fp;
		}
	}

	for (int i = 0; i < FEATURE_NUM / SF_NUM; ++i) {
		std::sort(feature + i * SF_NUM, feature + (i + 1) * SF_NUM);
	}

	// std::cout << "[";
    // for (size_t i = 0; i < FEATURE_NUM; ++i) {
    //     std::cout << feature[i];
    //     if (i != FEATURE_NUM - 1) {
    //         std::cout << ", ";
    //     }
    // }
    // std::cout << "]" << std::endl;


	for (int i = 0; i < SF_NUM; ++i) {
		feature_t temp[FEATURE_NUM / SF_NUM];
		for (int j = 0; j < FEATURE_NUM / SF_NUM; ++j) {
			temp[j] = feature[j * SF_NUM + i];
		}
		superfeature[i] = XXH32(temp, sizeof(feature_t) * FEATURE_NUM / SF_NUM, 0);
	}

	// std::cout << "[";
    // for (size_t i = 0; i < SF_NUM; ++i) {
    //     std::cout << superfeature[i];
    //     if (i != SF_NUM - 1) {
    //         std::cout << ", ";
    //     }
    // }
    // std::cout << "]" << std::endl;

	// uint32_t r = full_uint32_t(gen2) % SF_NUM;
	for (int index = 0; index < SF_NUM; ++index) {
		for (int j = 0; j < SF_NUM; ++j) {
			if (sfTable[index].count(superfeature[j])) {
				return sfTable[index][superfeature[j]].back();
			}
		}
	}
	
    // for (const auto& item : sfTable) {
    //     const auto& item_superfeatures = item.first;
        
    //     // 检查是否有任何一个 superfeature 匹配
    //     for (const auto& sf : item_superfeatures) {
	// 		for( int i = 0; i < SF_NUM; ++i )
	// 			if (sf == superfeature[i]) {
	// 				return item.second;  // 返回匹配项的编号
	// 			}
    //     }
    // }

	return -1;
}

// // insert "prev calculated" sf: label
// void Finesse::insert(int label) {
// 	std::vector<uint64_t> sfs;
// 	for (int i = 0; i < SF_NUM; ++i) {
// 		sfs.push_back(superfeature[i]);
// 		// sfTable[i][superfeature[i]].push_back(label);
// 	}

// 	std::pair<std::vector<uint64_t>, int> p (sfs, label);
// 	sfTable.push_back(p);
// }

void Finesse::insert(int label) {
	for (int i = 0; i < SF_NUM; ++i) {
		sfTable[i][superfeature[i]].push_back(label);
	}
}
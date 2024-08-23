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

class Odess {
	private:
	std::mt19937 gen1, gen2;
	std::uniform_int_distribution<uint32_t> full_uint32_t;

	int BLOCK_SIZE, W;
	int SF_NUM, FEATURE_NUM;

	uint32_t* TRANSPOSE_M;
	uint32_t* TRANSPOSE_A;

	const uint32_t A = 37, MOD = 1000000007;
	uint64_t Apower = 1;

	uint64_t mask;

	uint32_t* feature;
	uint64_t* superfeature;

	std::map<uint64_t, std::vector<int>>* sfTable;
	// std::vector< std::pair< std::vector<uint64_t>, int> > sfTable;
	public:
	Odess(int _BLOCK_SIZE, int _W, int _SF_NUM, int _FEATURE_NUM) {
		gen1 = std::mt19937(922);
		gen2 = std::mt19937(314159);
		full_uint32_t = std::uniform_int_distribution<uint32_t>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());

		mask = 0x00000007f0000000; // 0x00000007f0000000
		BLOCK_SIZE = _BLOCK_SIZE;
		W = _W;
		SF_NUM = _SF_NUM;
		FEATURE_NUM = _FEATURE_NUM;

		TRANSPOSE_M = new uint32_t[FEATURE_NUM];
		TRANSPOSE_A = new uint32_t[FEATURE_NUM];

		feature = new uint32_t[FEATURE_NUM];
		superfeature = new uint64_t[SF_NUM];

		sfTable = new std::map<uint64_t, std::vector<int>>[SF_NUM];

		for (int i = 0; i < FEATURE_NUM; ++i) {
			TRANSPOSE_M[i] = ((full_uint32_t(gen1) >> 1) << 1) + 1;
			TRANSPOSE_A[i] = full_uint32_t(gen1);
		}
		for (int i = 0; i < W - 1; ++i) {
			Apower *= A;
			Apower %= MOD;
		}
	}
	~Odess() {
		delete[] TRANSPOSE_M;
		delete[] TRANSPOSE_A;
		delete[] feature;
		delete[] superfeature;
		// delete[] sfTable;
	}
	int request(unsigned char* ptr, int size);
	void insert(int label);
};

// Gear hash
int Odess::request(unsigned char* ptr, int size) {
	for (int i = 0; i < FEATURE_NUM; ++i) feature[i] = 0;
	for (int i = 0; i < SF_NUM; ++i) superfeature[i] = 0;

	uint64_t fp = 0;

	int count = 0;
	for (int m = 0; m < size; ++m) {
		
		fp = (fp << 1) + (GEARv2[ptr[m]]);
		// if (fp > feature[i]) feature[i] = fp;
        
        if ((fp & mask) == 0) {  // 检查掩码条件 & | ~
			count ++;
            for (int i = 0; i < FEATURE_NUM; ++i) {
                uint32_t Transform = (TRANSPOSE_M[i] * fp + TRANSPOSE_A[i]) % (1UL << 32);  // 计算变换
                if (feature[i] <= Transform) {
                    feature[i] = Transform;  // 更新特征
                }
            }
        }
    }

	// std::cout << "count= " << count << std::endl;
	

	for (int i = 0; i < SF_NUM; ++i) {
		uint64_t temp[FEATURE_NUM / SF_NUM];
		for (int j = 0; j < FEATURE_NUM / SF_NUM; ++j) {
			temp[j] = feature[i * (FEATURE_NUM / SF_NUM) + j];
		}
		superfeature[i] = XXH64(temp, sizeof(uint64_t) * FEATURE_NUM / SF_NUM, 0);
	}

	// uint32_t r = full_uint32_t(gen2) % SF_NUM;
	// for (int i = 0; i < SF_NUM; ++i) {
	// 	int index = (r + i) % SF_NUM;
	// 	for (int j = 0; j < SF_NUM; ++j) {
	// 		if (sfTable[index].count(superfeature[j])) {
	// 			return sfTable[index][superfeature[j]].back();
	// 		}
	// 	}
	// }

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

// int Odess::request(unsigned char* ptr, int size) {
// 	for (int i = 0; i < FEATURE_NUM; ++i) feature[i] = 0;
// 	for (int i = 0; i < SF_NUM; ++i) superfeature[i] = 0;

// 	int64_t fp = 0;

// 	for(int j = 0; j < W; ++j){
// 		fp *= A;
// 		fp += (unsigned char)ptr[j]; // 计算滚动哈希
// 		fp %= MOD;
// 	}

// 	for (int m = 0; m < size - W + 1; ++m) {
		
// 		// if (fp > feature[i]) feature[i] = fp;
        
//         if ((fp & mask) == 0) {  // 检查掩码条件
//             for (int i = 0; i < FEATURE_NUM; ++i) {
//                 uint32_t Transform = (TRANSPOSE_M[i] * fp + TRANSPOSE_A[i]) % (1UL << 32);  // 计算变换
//                 if (feature[i] <= Transform) {
//                     feature[i] = Transform;  // 更新特征
//                 }
//             }
//         }

// 		fp -= (ptr[m] * Apower) % MOD;
// 		while (fp < 0) fp += MOD;
// 		if (m != size - W) {
// 			fp *= A;
// 			fp += ptr[m + W];
// 			fp %= MOD;
// 		}
//     }
	

// 	for (int i = 0; i < SF_NUM; ++i) {
// 		uint64_t temp[FEATURE_NUM / SF_NUM];
// 		for (int j = 0; j < FEATURE_NUM / SF_NUM; ++j) {
// 			temp[j] = feature[i * (FEATURE_NUM / SF_NUM) + j];
// 		}
// 		superfeature[i] = XXH64(temp, sizeof(uint64_t) * FEATURE_NUM / SF_NUM, 0);
// 	}

//     // for (const auto& item : sfTable) {
//     //     const auto& item_superfeatures = item.first;
        
//     //     // 检查是否有任何一个 superfeature 匹配
//     //     for (const auto& sf : item_superfeatures) {
// 	// 		for( int i = 0; i < SF_NUM; ++i )
// 	// 			if (sf == superfeature[i]) {
// 	// 				return item.second;  // 返回匹配项的编号
// 	// 			}
//     //     }
//     // }
// 	uint32_t r = full_uint32_t(gen2) % SF_NUM;
// 	for (int i = 0; i < SF_NUM; ++i) {
// 		int index = (r + i) % SF_NUM;
// 		for (int j = 0; j < SF_NUM; ++j) {
// 			if (sfTable[index].count(superfeature[j])) {
// 				return sfTable[index][superfeature[j]].back();
// 			}
// 		}
// 	}

// 	return -1;
// }

// insert "prev calculated" sf: label
// void Odess::insert(int label) {
// 	std::vector<uint64_t> sfs;
// 	for (int i = 0; i < SF_NUM; ++i) {
// 		sfs.push_back(superfeature[i]);
// 		// sfTable[i][superfeature[i]].push_back(label);
// 	}

// 	std::pair<std::vector<uint64_t>, int> p (sfs, label);
// 	sfTable.push_back(p);
// }

void Odess::insert(int label) {
	for (int i = 0; i < SF_NUM; ++i) {
		sfTable[i][superfeature[i]].push_back(label);
	}
}
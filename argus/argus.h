#include <iostream>
#include <tuple>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <random>
#include "../xxhash.h"
#include "../fastcdc/fastcdc.h"



class Argus {
	private:
	std::mt19937 gen1, gen2;
	std::uniform_int_distribution<uint32_t> full_uint32_t;

	int BLOCK_SIZE, WINDOW;
	int FEATURE_NUM;

	uint32_t* TRANSPOSE_M;
	uint32_t* TRANSPOSE_A;

	int byte_shift;

	const uint32_t A = 37, MOD = 1000000007;
	uint64_t Apower = 1;

	uint64_t mask;

	uint32_t* feature;

	std::map<uint32_t, std::vector<int>>* sfTable;
	// std::vector< std::pair< std::vector<uint64_t>, int> > sfTable;
	public:
	Argus(int _BLOCK_SIZE, int _byte_shift, int _FEATURE_NUM) {
		gen1 = std::mt19937(922);
		gen2 = std::mt19937(314159);
		full_uint32_t = std::uniform_int_distribution<uint32_t>(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());

		mask = 0x00000007f0000000; // 0x00000007f0000000
		BLOCK_SIZE = _BLOCK_SIZE;
		FEATURE_NUM = _FEATURE_NUM;

		byte_shift = _byte_shift;

		TRANSPOSE_M = new uint32_t[FEATURE_NUM];
		TRANSPOSE_A = new uint32_t[FEATURE_NUM];

		feature = new uint32_t[FEATURE_NUM];

		sfTable = new std::map<uint32_t, std::vector<int>>[FEATURE_NUM];

		for (int i = 0; i < FEATURE_NUM; ++i) {
			TRANSPOSE_M[i] = ((full_uint32_t(gen1) >> 1) << 1) + 1;
			TRANSPOSE_A[i] = full_uint32_t(gen1);
		}
		for (int i = 0; i < WINDOW - 1; ++i) {
			Apower *= A;
			Apower %= MOD;
		}
	}
	~Argus() {
		delete[] TRANSPOSE_M;
		delete[] TRANSPOSE_A;
		delete[] feature;
		// delete[] sfTable;
	}
	int request(unsigned char* ptr, int size);
	void insert(int label);
	int calculateFeatureMatches();
};

// 函数：获取uint8最后4位并转换为0-2的范围
int getValueFromLast4Bits(uint8_t num) {
    // 使用掩码0x0F提取最后4位
    uint8_t last4Bits = num & 0x0F;  // 0x0F = 00001111

    // 通过除以5进行简化分类
    return last4Bits / 5;
}

// 函数：计算出现次数最多的ID
int Argus::calculateFeatureMatches() {
    std::unordered_map<int, int> idFrequency;  // 用于统计每个ID的出现次数

    // 遍历每个特征
    for (int i = 0; i < FEATURE_NUM; ++i) {
        uint32_t feat = feature[i];

        // 查找特征是否存在于sfTable中
        auto it = sfTable[i].find(feat);
        if (it != sfTable[i].end()) {
            // 获取与该特征关联的ID列表
            const std::vector<int>& idList = it->second;

            // 遍历ID列表，统计每个ID的出现次数
            for (int id : idList) {
                idFrequency[id]++;
            }
        }
    }

    // 找到出现次数最多的ID
    int mostFrequentID = -1;
    int maxFrequency = 0;
    for (const auto& entry : idFrequency) {
        if (entry.second > maxFrequency) {
            maxFrequency = entry.second;
            mostFrequentID = entry.first;
        }
    }

    return mostFrequentID;  // 返回出现次数最多的ID
}

// Gear hash
int Argus::request(unsigned char* ptr, int size) {
	for (int i = 0; i < FEATURE_NUM; ++i) feature[i] = 0;

	uint64_t fp = 0;

	int count = 0;
	for (int m = 0; m < size; ++m) {
		
		fp = (fp << byte_shift) + (GEARv2[ptr[m]]);
		// if (fp > feature[i]) feature[i] = fp;

        if ((fp & mask) == 0) {  // 检查掩码条件 & | ~
			count ++;
			int bin = getValueFromLast4Bits(ptr[m]);
            uint32_t Transform = (TRANSPOSE_M[bin] * fp + TRANSPOSE_A[bin]) % (1UL << 32);  // 计算变换
			if (feature[bin] <= Transform) {
				feature[bin] = Transform;  // 更新特征
			}
        }
    }

	// std::cout << "count= " << count << std::endl;
	

	uint32_t r = full_uint32_t(gen2) % FEATURE_NUM;
	for (int i = 0; i < FEATURE_NUM; ++i) {
		int index = (r + i) % FEATURE_NUM;
		for (int j = 0; j < FEATURE_NUM; ++j) {
			if (sfTable[index].count(feature[j])) {
				return sfTable[index][feature[j]].back();
			}
		}
	}

    // return calculateFeatureMatches();  // 返回匹配特征最多的候选块ID
}

void Argus::insert(int label) {
	for (int i = 0; i < FEATURE_NUM; ++i) {
		sfTable[i][feature[i]].push_back(label);
	}
}
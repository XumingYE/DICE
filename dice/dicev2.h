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
#include "../fastcdc/fastcdc.h"
#include "../annoy/src/annoylib.h"
#include "../annoy/src/kissrandom.h"
#include "../xxhash.h"
// #include "annoylib.h"
// dice
// #define DIMENSION 48
#define THREADS 8
// #define ANNOYLIB_MULTITHREADED_BUILD 1
typedef float ANNOYTYPE;
typedef Annoy::AnnoyIndex<int, ANNOYTYPE, Annoy::Euclidean, Annoy::Kiss64Random, Annoy::AnnoyIndexMultiThreadedBuildPolicy> MyAnnoyIndex;

// using namespace Annoy;

class DICE {
	private:
	int ANN_SEARCH_CNT, LINEAR_SIZE, DIMENSION;
    double THRESHOLD;
	std::vector<std::pair<int, arma::frowvec>> linear; // linear成员
	// std::map<uint64_t, std::vector<int>>* sfTable;

	// Annoy
	std::default_random_engine generator;
	std::normal_distribution<float> distribution;
	MyAnnoyIndex indexing;
    bool built;

	// Byte Slice Structure
    int NUMBER_OF_BYTE_SLICE;
	std::vector<int> byte_slice;
	std::vector<arma::frowvec> byte_vectors; // 存储所有字节对应的向量
    arma::frowvec avg_embedding;
    // Cache

    // ODESS
    uint64_t mask;
	public:
	DICE(int DIMENSION, int LINEAR_SIZE, double THRESHOLD, std::vector<int> byte_slice):indexing(DIMENSION) {
		this->DIMENSION = DIMENSION; // The number of candidates extract from ANN class
		this->LINEAR_SIZE = LINEAR_SIZE; // Size of linear buffer
		this->THRESHOLD = THRESHOLD;
		this->byte_slice = byte_slice;
        
		this->distribution = std::normal_distribution<float>(-1.0f, 1.0f);
        this->NUMBER_OF_BYTE_SLICE = byte_slice.size();
        // this->avg_embedding = std::vector<ANNOYTYPE>(DIMENSION, 0.0f);
        // indexing.build(2 * DIMENSION, THREADS);
        // indexing(DIMENSION);
        this->built = false;

        // 初始化byte_vectors
        byte_vectors.resize(256);

        for (int i = 0; i < 256; ++i) {
            // 创建一个维度为DIMENSION的随机向量
            arma::frowvec vec = arma::randu<arma::frowvec>(DIMENSION);
            byte_vectors[i] = vec; // 将向量存储到byte_vectors中
        }

        mask = 0x00000007f0000000; // 0x00000007f0000000
	}
    ~DICE() {
		// delete[] gram_dict;
	}
	int request(unsigned char *ptr, int chunk_size);
	void insert(int label);
	std::vector<std::string> generate_ngrams(unsigned char *ptr, int chunk_size, int n, int step = 1);
	arma::frowvec get_embedding(unsigned char *ptr, int chunk_size);
	// std::vector<ANNOYTYPE> generate_vector_from_hash(const std::string &gram);
    double euclidean_distance(const arma::frowvec& vec1, const arma::frowvec& vec2); // 计算欧几里得距离
};



// 获取嵌入向量的实现
// arma::frowvec DICE::get_embedding(unsigned char *ptr, int chunk_size) {
//     arma::frowvec avg_vector(DIMENSION, arma::fill::zeros); // 初始化平均向量为零向量

//     for (int i = 0; i < chunk_size; ++i) {
//         unsigned char byte = ptr[i];
//         avg_vector += byte_vectors[byte]; // 累加对应的向量
//     }

//     if (chunk_size > 0) {
//         avg_vector /= chunk_size; // 计算平均值
//     }

//     return avg_vector;
// }

// arma::frowvec DICE::get_embedding(unsigned char *ptr, int chunk_size) {
//     int byte_count[256] = {0}; // 使用静态数组代替std::vector
//     arma::frowvec sum_vector(DIMENSION, arma::fill::zeros);

//     // 统计每个byte出现的次数
//     for (int i = 0; i < chunk_size; ++i) {
//         unsigned char byte = ptr[i];
//         byte_count[byte]++;
//     }

//     // 并行处理向量加权和
//     #pragma omp parallel
//     {
//         arma::frowvec local_sum(DIMENSION, arma::fill::zeros);  // 每个线程的局部 sum_vector

//         #pragma omp for
//         for (int i = 0; i < 256; ++i) {
//             if (byte_count[i] > 0) {
//                 if (byte_count[i] == 1) {
//                     local_sum += byte_vectors[i]; // 避免乘法
//                 } else {
//                     local_sum += byte_count[i] * byte_vectors[i];
//                 }
//             }
//         }

//         #pragma omp critical
//         sum_vector += local_sum;  // 将局部结果累加到全局结果中
//     }

//     // 计算平均值
//     arma::frowvec avg_vector = sum_vector / chunk_size;

//     return avg_vector;
// }

arma::frowvec DICE::get_embedding(unsigned char *ptr, int chunk_size) {
    std::vector<int> byte_count(256, 0); // 初始化每个byte的计数为0
    arma::frowvec sum_vector(DIMENSION, arma::fill::zeros); // 初始化累加向量为零向量

    uint64_t fp = 0;

    /* Odess */
	// for (int m = 0; m < chunk_size; ++m) {
		
	// 	fp = (fp << 1) + (GEARv2[ptr[m]]);
	// 	// if (fp > feature[i]) feature[i] = fp;
        
    //     if ((fp & mask) == 0) {  // 检查掩码条件
    //         byte_count[ptr[m]]++;
    //     }
    // }

    /* 统计每个byte出现的次数 */ 
    for (int i = 0; i < chunk_size; ++i) {
        byte_count[ptr[i]]++;
    }

    // 根据出现次数累加向量
    for (int i = 0; i < 256; ++i) {
        if (byte_count[i] > 0)
            sum_vector += byte_count[i] * byte_vectors[i];
    }

    // 计算平均值
    // arma::frowvec avg_vector = sum_vector / chunk_size;

    return sum_vector / chunk_size;
}


// arma::frowvec DICE::get_embedding(unsigned char *ptr, int chunk_size) {
//     arma::frowvec avg_vector(DIMENSION, arma::fill::zeros);

//     // 创建一个行向量数组，每个线程一个
//     std::vector<arma::frowvec> local_sums(omp_get_max_threads(), arma::frowvec(DIMENSION, arma::fill::zeros));

//     #pragma omp parallel for
//     for (int i = 0; i < chunk_size; ++i) {
//         int thread_id = omp_get_thread_num();
//         unsigned char byte = ptr[i];
//         local_sums[thread_id] += byte_vectors[byte]; // 各线程累加自己的向量
//     }

//     // 合并所有线程的结果
//     for (auto &local_sum : local_sums) {
//         avg_vector += local_sum;
//     }

//     if (chunk_size > 0) {
//         avg_vector /= chunk_size; // 计算平均值
//     }

//     return avg_vector;
// }


// arma::frowvec DICE::get_embedding(unsigned char *ptr, int chunk_size) {
//     arma::frowvec sum_vector(DIMENSION, arma::fill::zeros); // 初始化累加向量为零向量

//     // 遍历每个字节，并加入位置编码
//     for (int i = 0; i < chunk_size; ++i) {
//         unsigned char byte = ptr[i];

//         // 为每个字节计算位置编码
//         arma::frowvec position_encoding(DIMENSION, arma::fill::zeros);
//         for (int j = 0; j < DIMENSION; ++j) {
//             // 线性位置编码，增加位置因子
//             position_encoding[j] = (i + 1) * 0.01; // 可以调整系数以适应不同的尺度需求
//         }

//         // 将位置编码添加到字节的向量表示中
//         sum_vector += byte_vectors[byte] + position_encoding;
//     }

//     // 计算平均值
//     arma::frowvec avg_vector = sum_vector / chunk_size;

//     return avg_vector;
// }

std::vector<std::string> DICE::generate_ngrams(unsigned char *ptr, int chunk_size, int n, int step) {
    std::vector<std::string> ngrams;
    // std::cout << "chunk size = " << chunk_size << std::endl;
    for (int i = 0; i <= chunk_size - n; i += step) {
        std::string ngram(reinterpret_cast<char*>(ptr + i), n);
        ngrams.push_back(ngram);
    }
    return ngrams;
}



int DICE::request(unsigned char *ptr, int chunk_size) {

    avg_embedding = get_embedding(ptr, chunk_size);

    double dist = 999.9;
	int ret = -1;

	// scan list
    if (linear.size() > 0)
        for (int i = linear.size() - 1; i >= 0; --i) {
            double nowdist = euclidean_distance(linear[i].second, avg_embedding);
            // std::cout << "Linear Index: " << linear[i].first << ", Distance: " << nowdist << "\n vector = "<< linear[i].second << std::endl;

            if (dist > nowdist) {
                dist = nowdist;
                ret = linear[i].first;
            }
        }

    if (built) {
        std::vector<int> closest;
        std::vector<ANNOYTYPE> distance;

        indexing.get_nns_by_vector(avg_embedding.memptr(), 1, -1, &closest, &distance);

        // 输出结果
        // std::cout << "Nearest neighbors:" << std::endl;
        // for (size_t i = 0; i < closest.size(); ++i) {
        //     std::cout << "Index: " << closest[i] << ", Distance: " << distance[i] << std::endl;
        //     if (dist > distance[i]) {
        //         dist = distance[i];
        //         ret = closest[i];
        //     }
        // }
        // std::cout << "Index: " << closest[0] << ", Distance: " << distance[0] << std::endl;
        if (dist > distance[0]) {
            // std::cout << "dist: " << dist << std::endl;
            // std::cout << "Index: " << closest[0] << ", Distance: " << distance[0] << std::endl;
            // std::cout << "Shot\n";
            dist = distance[0];
            ret = closest[0];
        }
    }
    // std::cout << "dist: " << dist << std::endl;
    if (dist <= THRESHOLD) return ret;
	else return -1;
    // 这里返回 -1 仅作为示例，实际你可能需要返回向量或其他信息
}

// insert "prev calculated" sf: label
void DICE::insert(int label) {
    // if (!built) {
        linear.emplace_back(label, avg_embedding);
        // std::cout << "size of linear: " << linear.size() << std::endl;
        // indexing.unbuild();
        if (linear.size() == LINEAR_SIZE) {
            for (const auto& pair : linear) {
                // std::cout << "Push\n";
                indexing.add_item(pair.first, pair.second.memptr());
            }
            linear.clear();
            if (!built) {
                indexing.build(2 * DIMENSION, THREADS);
                built = true;
            }
	    }
    // }
    // else {
    //     indexing.add_item(label, avg_embedding.memptr());
    // }
    // std::cout << "Annoy numbers = " << indexing.get_n_items() << "\n";
}

inline double DICE::euclidean_distance(const arma::frowvec& vec1, const arma::frowvec& vec2) {
    return arma::norm(vec1 - vec2, 2); // 计算L2范数（欧几里得距离）
}

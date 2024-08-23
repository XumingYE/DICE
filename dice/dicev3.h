/**
 * 由于效果比加上Gear Hash的Odess要慢，所以没办法，必须加速
 * 
 * 1. annoy改为hnswlib
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
#include <thread>
#include <atomic>
#include <stdlib.h>
#include "../hnswlib/hnswlib.h"
#include "../fastcdc/fastcdc.h"
#include "../xxhash.h"
// #include "annoylib.h"
// dice
// #define DIMENSION 48
#define THREADS 8
// #define ANNOYLIB_MULTITHREADED_BUILD 1
typedef float KNNTYPE;


// using namespace Annoy;

template<class Function>
inline void ParallelFor(size_t start, size_t end, size_t numThreads, Function fn) {
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
    }

    if (numThreads == 1) {
        for (size_t id = start; id < end; id++) {
            fn(id, 0);
        }
    } else {
        std::vector<std::thread> threads;
        std::atomic<size_t> current(start);

        // keep track of exceptions in threads
        // https://stackoverflow.com/a/32428427/1713196
        std::exception_ptr lastException = nullptr;
        std::mutex lastExceptMutex;

        for (size_t threadId = 0; threadId < numThreads; ++threadId) {
            threads.push_back(std::thread([&, threadId] {
                while (true) {
                    size_t id = current.fetch_add(1);

                    if (id >= end) {
                        break;
                    }

                    try {
                        fn(id, threadId);
                    } catch (...) {
                        std::unique_lock<std::mutex> lastExcepLock(lastExceptMutex);
                        lastException = std::current_exception();
                        /*
                         * This will work even when current is the largest value that
                         * size_t can fit, because fetch_add returns the previous value
                         * before the increment (what will result in overflow
                         * and produce 0 instead of current + 1).
                         */
                        current = end;
                        break;
                    }
                }
            }));
        }
        for (auto &thread : threads) {
            thread.join();
        }
        if (lastException) {
            std::rethrow_exception(lastException);
        }
    }
}

class DICE {
	private:
	int ANN_SEARCH_CNT, LINEAR_SIZE, DIMENSION;
    double THRESHOLD;
	std::vector<std::pair<int, arma::frowvec>> linear; // linear成员
	// std::map<uint64_t, std::vector<int>>* sfTable;

	// Annoy
	int max_elements;
    hnswlib::L2Space space;
    hnswlib::HierarchicalNSW<float>* alg_hnsw;

	// Byte Slice Structure
    int NUMBER_OF_BYTE_SLICE;
	std::vector<int> byte_slice;
	std::vector<arma::frowvec> byte_vectors; // 存储所有字节对应的向量
    arma::frowvec avg_embedding;
    // Cache

    // ODESS
    uint64_t mask;
	public:
	DICE(int DIMENSION, int LINEAR_SIZE, double THRESHOLD, int max_elements):space(DIMENSION) {
		this->DIMENSION = DIMENSION; // The number of candidates extract from ANN class
		this->LINEAR_SIZE = LINEAR_SIZE; // Size of linear buffer
		this->THRESHOLD = THRESHOLD;
		this->byte_slice = byte_slice;
        
        // knn
        this->max_elements = max_elements;
        int M = 5;                 // Tightly connected with internal dimensionality of the data
                                // strongly affects the memory consumption
        int ef_construction = 50;  // Controls index search speed/build speed tradeoff
        this->alg_hnsw = new hnswlib::HierarchicalNSW<KNNTYPE>(&space, max_elements, DIMENSION, ef_construction);
        this->alg_hnsw->setEf(10);

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
        delete alg_hnsw;
		// delete[] gram_dict;
	}
	int request(unsigned char *ptr, int chunk_size);
	void insert(int label);
	arma::frowvec get_embedding(unsigned char *ptr, int chunk_size);
    double euclidean_distance(const arma::frowvec& vec1, const arma::frowvec& vec2); // 计算欧几里得距离
};



arma::frowvec DICE::get_embedding(unsigned char *ptr, int chunk_size) {
    std::vector<int> byte_count(256, 0); // 初始化每个byte的计数为0
    arma::frowvec sum_vector(DIMENSION, arma::fill::zeros); // 初始化累加向量为零向量

    /* 统计每个byte出现的次数 */
    for (int i = 0; i < chunk_size; ++i) {
        byte_count[ptr[i]]++;
    }
    
    // 根据出现次数累加向量
    for (int i = 0; i < 256; ++i) {
        if (byte_count[i] > 0) {
            sum_vector += byte_count[i] * byte_vectors[i];
        }
    }

    // 计算平均值
    // arma::frowvec avg_vector = sum_vector / total_count;

    return sum_vector / chunk_size;
}




int DICE::request(unsigned char *ptr, int chunk_size) {

    avg_embedding = get_embedding(ptr, chunk_size);
    return 0;
    // double dist = 999.9;
	// int ret = -1;

    // if (linear.size() > 0)
    //     for (int i = linear.size() - 1; i >= 0; --i) {
    //         double nowdist = euclidean_distance(linear[i].second, avg_embedding);
    //         // std::cout << "Linear Index: " << linear[i].first << ", Distance: " << nowdist << "\n vector = "<< linear[i].second << std::endl;
    //         if (dist > nowdist) {
    //             dist = nowdist;
    //             ret = linear[i].first;
    //         }
    //     }

    // if (alg_hnsw->cur_element_count > 0) {
    //     std::priority_queue<std::pair<float, size_t>> result = alg_hnsw->searchKnn(avg_embedding.memptr(), 1);

    //     // 输出结果
    //     // sqrt()
    //     // std::cout << "Nearest neighbors:" << std::endl;
    //     if (dist > sqrt(result.top().first)) {
    //         dist = sqrt(result.top().first);
    //         ret = result.top().second;
    //         // std::cout << "Shot:" << std::endl;
    //     }
    // }

    // if (dist <= THRESHOLD) return ret;
	// else return -1;
    // 这里返回 -1 仅作为示例，实际你可能需要返回向量或其他信息
}

// insert "prev calculated" sf: label
void DICE::insert(int label) {
	linear.emplace_back(label, avg_embedding);
    if (linear.size() == LINEAR_SIZE) {
        for (const auto& pair : linear) {
            if (alg_hnsw->cur_element_count < alg_hnsw->max_elements_)
                alg_hnsw->addPoint(pair.second.memptr(), pair.first);
        }
        // ParallelFor(0, LINEAR_SIZE, 8, [&](size_t row, size_t threadId) {
        //             alg_hnsw->addPoint(linear[row].second.memptr(), linear[row].first);
        //             });
		linear.clear();
	}
}

inline double DICE::euclidean_distance(const arma::frowvec& vec1, const arma::frowvec& vec2) {
    return arma::norm(vec1 - vec2, 2); // 计算L2范数（欧几里得距离）
}



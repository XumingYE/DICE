/**
 * This is an implementation of fastCDC
 * The origin paper is Wen Xia, Yukun Zhou, Hong Jiang, Dan Feng, Yu Hua, Yuchong Hu, Yucheng Zhang, Qing Liu, "FastCDC: a Fast and Efficient Content-Defined Chunking Approach for Data Deduplication", in Proceedings of USENIX Annual Technical Conference (USENIX ATC'16), Denver, CO, USA, June 22–24, 2016, pages: 101-114
 * and Wen Xia, Xiangyu Zou, Yukun Zhou, Hong Jiang, Chuanyi Liu, Dan Feng, Yu Hua, Yuchong Hu, Yucheng Zhang, "The Design of Fast Content-Defined Chunking for Data Deduplication based Storage Systems", IEEE Transactions on Parallel and Distributed Systems (TPDS), 2020
 *
 */ 
#ifndef FASTCDC_H
#define FASTCDC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <zlib.h>
#include "uthash.h"

#define USE_CHUNKING_METHOD 1

// predefined Gear Mask

#define SymbolCount 256
#define SeedLength 64
#define CacheSize 1024 * 1024 * 1024

#define ORIGIN_CDC 1
#define ROLLING_2Bytes 2
#define NORMALIZED_CDC 3
#define NORMALIZED_2Bytes 4
#define DEFAULT_AVG_SIZE 8192

// Define the Chunk structure
typedef struct {
    int id;
    size_t size;
    size_t offset;
    unsigned char *data;
} Chunk;

extern uint64_t GEARv2[256];

extern Chunk* process_file_chunks(const char* file_path, size_t _min_size, size_t _max_size, size_t _avg_size, int* chunkCount);



#ifdef __cplusplus
}
#endif

#endif // FASTCDC_H
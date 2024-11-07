#include <vector>
#include <bitset>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "fastcdc/fastcdc.h"
#define BLOCK_SIZE 8192

// offset: 40bit (~1TB)
// size: 12bit (~4KB)
// ref: 28bit (1TB / 4KB)
// flag: 2bit (00: not-compressed, 01: self-compressed, 10: deduped, 11: delta-compressed)
// total: 40 + 12 + 28 + 2 = 82bit

typedef std::bitset<82> RECIPE;

static inline void set_offset(RECIPE& r, unsigned long long t) { r |= (RECIPE(t) << 42); }
static inline void set_size(RECIPE& r, unsigned long t) { r |= (RECIPE(t) << 30); }
static inline void set_ref(RECIPE& r, unsigned long t) { r |= (RECIPE(t) << 2); }
static inline void set_flag(RECIPE& r, unsigned long t) { r |= (RECIPE(t) << 0); }
static inline unsigned long long get_offset(RECIPE& r) { return ((r << 0) >> 42).to_ullong(); }
static inline unsigned long get_size(RECIPE& r) { return ((r << 40) >> 70).to_ulong(); }
static inline unsigned long get_ref(RECIPE& r) { return ((r << 52) >> 54).to_ulong(); }
static inline unsigned long get_flag(RECIPE& r) { return ((r << 80) >> 80).to_ulong(); }



struct DATA_IO {
	int N;
	std::vector<Chunk> trace;
	std::vector<RECIPE> recipe;

	char fileName[100];
	char outputFileName[100];
	char recipeName[100];

	FILE* out;

	long size;

	struct timeval start_time, end_time;

	DATA_IO(char* name);
	~DATA_IO(){
		for (auto& chunk : trace) {
			if (chunk.data != nullptr) {
				free(chunk.data);
				chunk.data = nullptr;
			}
		}
	};
	void read_file(int avg_size);
	inline void write_file(char* data, int size);
	inline void write_file(unsigned char* data, int size);
	inline void recipe_insert(RECIPE r);
	void recipe_write();
	void time_check_start();
	long long time_check_end();
};

DATA_IO::DATA_IO(char* name) {
	sprintf(fileName, "%s", name);
	sprintf(outputFileName, "%s_output", name);
	sprintf(recipeName, "%s_recipe", name);

	out = NULL;
}

// DATA_IO::~DATA_IO() {
// 	if (out) fclose(out);

// 	for (int i = 0; i < N; ++i) {
// 		free(trace[i].data);
// 	}

// }

// 打印为字符
void printAsChars(const unsigned char *ptr, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        std::cout << static_cast<char>(ptr[i]);
    }
    std::cout << std::endl;
}



void DATA_IO::read_file(int avg_size) {
	N = 0;
	trace.clear();

    Chunk* chunks = process_file_chunks(fileName, 0, 0, avg_size, &N);
	// printAsChars(chunks[0].data, chunks[0].size);
	trace.assign(chunks, chunks + N);
	// 释放原始的 C 风格数组
    free(chunks);

	struct stat file_stat;
	stat(fileName, &file_stat);
	size = file_stat.st_size;
	// std::cout << "trace size: " << trace.size() << std::endl;
}

inline void DATA_IO::write_file(char* data, int size) {
	if (out == NULL) out = fopen(outputFileName, "wb");
	fwrite(data, size, 1, out);
}

inline void DATA_IO::write_file(unsigned char* data, int size) {
	if (out == NULL) out = fopen(outputFileName, "wb");
	fwrite(data, size, 1, out);
}

inline void DATA_IO::recipe_insert(RECIPE r) {
	recipe.push_back(r);
}

void DATA_IO::recipe_write() {
	FILE* f = fopen(recipeName, "wb");
	for (int i = 0; i < N; ++i) {
		fwrite(&recipe[i], 1, sizeof(RECIPE), f);
	}
	fclose(f);
}

void DATA_IO::time_check_start() {
	gettimeofday(&start_time, NULL);
}

long long DATA_IO::time_check_end() {
	gettimeofday(&end_time, NULL);
	return (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);
}

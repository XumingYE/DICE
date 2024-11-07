#include <iostream>
#include <vector>
#include <cstring>
#include "../xdelta3/xdelta3.h"
#include <fstream>

unsigned char* readFile(const std::string& filename, size_t& fileSize) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return nullptr;
    }

    // 获取文件大小
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 动态分配数组并读取文件内容
    unsigned char* buffer = new unsigned char[fileSize];
    if (file.read(reinterpret_cast<char*>(buffer), fileSize)) {
        return buffer;
    } else {
        std::cerr << "Failed to read file: " << filename << std::endl;
        delete[] buffer;  // 释放已分配的内存
        return nullptr;
    }
}


int main() {
    // const unsigned char originalData[] = "Hello, World!";
    // const unsigned char  modifiedData[] = "Hello, all!";
    size_t originalSize = 0;
    size_t modifiedSize = 0;

    // 从文件读取原始数据
    unsigned char* originalData = readFile("test/ori.txt", originalSize);
    if (!originalData) {
        return 1; // 文件读取失败
    }

    // 从文件读取修改后的数据
    unsigned char* modifiedData = readFile("test/moi.txt", modifiedSize);
    if (!modifiedData) {
        delete[] originalData;  // 释放已分配的内存
        return 1; // 文件读取失败
    }

    size_t output_alloc = int(modifiedSize * 1.2);
    uint8_t delta[output_alloc];
    usize_t output_size = 0;
    

    /* Reset Xdelta3 with no secondary compression*/
	xd3_config config;
    memset(&config, 0, sizeof(config));

    // 设置配置参数，禁用内置压缩和二次压缩
    config.flags |= XD3_NOCOMPRESS;   // 禁用内置的普通压缩算法
    // config.flags |= XD3_SEC_NOALL;    // 禁用二次压缩的所有部分
	// config.flags |= XD3_COMPLEVEL_9;

    // 确保未启用任何二次压缩算法
    config.flags &= ~(XD3_SEC_DJW | XD3_SEC_FGK | XD3_SEC_LZMA);

    // int ret = xd3_encode_memory(modifiedData, sizeof(modifiedData), originalData, sizeof(originalData), delta, &output_size, output_alloc, config.flags);
    int ret = xd3_encode_memory(modifiedData, modifiedSize, originalData, originalSize, delta, &output_size, output_alloc, config.flags);

    std::cout << "ret=" << ret << std::endl;
    if (output_size >= 0) {
        std::cout << "Delta Size: " << output_size << " bytes" << std::endl;
    } else {
        std::cout << "Failed to compute delta." << std::endl;
    }

    return 0;
}
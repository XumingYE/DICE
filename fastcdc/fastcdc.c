#include <sys/stat.h>
#include "fastcdc.h"
// USE_CHUNKING_METHOD


uint64_t GEARv2[256] = {
    0xdc377e207d3c5d43, 0x626790b237a4ab52, 0xfad9bf3a472cfe4d,
    0xa2a6bc5395bbce52, 0xce0a8e4ef2f3ee3f, 0xb4b5b36cf31b4d66,
    0x468f4da9d12660f7, 0x9005e918384c13d0, 0x29ba77f861e74697,
    0x8237cf1734b2c668, 0xee06f9f1df6ced7b, 0x142936b9399add6a,
    0xcb3c0a27878d6de5, 0x49b827acbe6d77ac, 0x65903aad1d6c1e9f,
    0x7a7c66f221cf20ca, 0x9a0f5565415a795,  0xbe5a882391e5527e,
    0x14d2a06077f8339d, 0xcad2bce34bf19652, 0x5541cc588464c621,
    0xc44d981a4aa21e70, 0xb5f4dbb200e75029, 0xa0fc23f2eef70334,
    0xfe36dbfaed334ef3, 0xdd1b785765b2bb6,  0xcc3461a160502f47,
    0x64b1c122c7083b1a, 0x388e6a418b6fd359, 0x65809585f16ab490,
    0x3eb1ec4b9577e4c7, 0xd80e411261617dd8, 0x7da08fd8df71e169,
    0xa20af93edb933a86, 0x59fb1b86e1ff5415, 0x5b596e7a23e7af9a,
    0x50a905ffc0a402c3, 0x3a8cefabefd9dbbe, 0xc9762597ccab4a09,
    0xd8de3774872f3efe, 0xc9f1075d9ebc3c51, 0x3b55ac8bfcf8f51e,
    0xe4e88fb0fa555bd1, 0xfa725ab5e019900a, 0x4be7597f3fcb499d,
    0xe108b91410eb4788, 0xb9f7fc4896cbf4c3, 0x8cdaddac2fc852ee,
    0x5c1d3439307f3d03, 0x60e5ae5e93c82d50, 0x340d48d6be8c5b09,
    0x9a02c3be5b6c05e8, 0x56014afc31084d83, 0x85b888f604ea56f2,
    0x5cb1a8a10079cb23, 0xc52b57c63ff9a0ca, 0xf35659fc64e0143,
    0xbdf325035c594f38, 0xd1f8101a35320afd, 0xe899458626d703f8,
    0xbf97f530ac049837, 0x895021a6620ae70,  0x948596cd24280401,
    0x471f6accca7627f4, 0x4a129ff1164598bd, 0x71405c568bef229e,
    0x4fda2755c3737887, 0x909a3cc70ee44b32, 0xd1a8a3c3dc9fd44d,
    0x5ebb36a043ede1ca, 0xf6a89a10d5e68b53, 0xd9bf7a1aaa5016b2,
    0x71030478353d66cb, 0x6806d8ba045b5ae0, 0xa7672b4808466f67,
    0xf93e0da0d5011f0a, 0xa8aeeeec63465029, 0xe958e539a532a76e,
    0xfd8bf78ab9628da1, 0xca1eb1fe8a769e6a, 0x2a96875d6c8f9257,
    0x501c523d559d7d12, 0x9b7e72aa3d7f1a65, 0x25554c1d398077a,
    0xcdd8af6cd9c471e1, 0x39bea30bd9a49e,   0x233b737e0eeee721,
    0xd6d57c6896b14ed4, 0x8dabe80e8c70e265, 0x1b859ed8c291ddbc,
    0x6b3385f41e0a598b, 0xcf05250d14390e94, 0xc58d5e7d9c8b9f73,
    0x49a5c7a9ba27febc, 0x4841a129f0804005, 0xe7ffa313ce3144a,
    0x13d8768f158b32cb, 0x5d4a2c0fe9cd7afe, 0xe5d0ae6ab568df33,
    0xd5e94a4243d9b506, 0xc6dc1d3da6a23d23, 0xf3b4295d0dd364f2,
    0x57750173991256b5, 0x119097313522fa6a, 0xbeecd02c90273c43,
    0xef69efdc30e9077e, 0xcf28f3bbba364c83, 0xc8b80c742bfdd966,
    0x83f12924c9400e15, 0xa35b3222d11d583e, 0x9c9a9d426cde5fdd,
    0xf298ed021e76023a, 0xb9e8b29602ced7f3, 0x853af80c4f919742,
    0x505b7d96bf01b253, 0xa318b2bff19ed50c, 0x5308029ce76f358f,
    0x9a398f26b33f24a,  0x74595977a450ca7b, 0x4e8468ce85680390,
    0x2436fba706b7bf67, 0xfc0923f5563e8424, 0x5fcbfacf4f88506b,
    0xbe722684e90680f2, 0x1119ab5f71bd737d, 0xc739c71894e34ba8,
    0xcd822b0cb4e2e159, 0x67b583610a0410e0, 0xae18b6eb024bdfdb,
    0x865951f3e76f5834, 0x18eebfb065ebcb59, 0xd35f0999dd5e5b00,
    0x241ad24ae452fe07, 0x942b4c3c79dd1dbe, 0x99b14f06198c22a7,
    0xe2987fcf99376312, 0x844daa9a945c9067, 0xfa234ecf470184dc,
    0x6d97ccd39c1eb593, 0x8bf12e40249118d0, 0x923f72bcdb934d2f,
    0x69bd5c907fc76c8e, 0xe5827868949fc4f3, 0x4f5d13c3d6e20e0a,
    0xb3646a9d0111233,  0xf0af68353b17c0c8, 0xc920f56447f90673,
    0x8960c8168641fe16, 0xfee91d454a19219,  0xb25b446e135cf6b2,
    0x761b04f2362314f9, 0xa151afe2fb1ff5be, 0x2e973d6af5de4037,
    0x485c4501258ab54c, 0xf21bd1e05d869951, 0xd79097aaa1050314,
    0x2b5e8c12e04ff4f9, 0x4e43a881e78d9764, 0x16d02eca685abdab,
    0x7913757d06ccfaec, 0x513242305e9af1ef, 0xc847965583801b62,
    0x8862452b0de8c5e5, 0xce5ae051740dea5a, 0x1a028ca4bb2875a3,
    0x5680bba4aad7ffa6, 0x324d2adfb43a331b, 0xe456b7b1c0301b68,
    0x7801a00c795d859d, 0x41bdd48db6ae14a,  0x5fa8107e14c841fb,
    0xd0e4bdea28bef85c, 0x77bdb5eb30614b89, 0xdefa9fd302bbd858,
    0x8a54bfa54688dad,  0x682ec11a915f0980, 0xc4af0b1c0ffec719,
    0xf76fd41604e89104, 0xeb01bf3d9ced6817, 0xa87180c091474c6e,
    0x351bdb3557277969, 0xee81b4ce09b723aa, 0x3bd353a36b05adb,
    0x7bcb44892055af78, 0xadfb4e960a0ca951, 0x2273bad9b4ac3d74,
    0xebb4454444aaa94b, 0xe686846acad641a8, 0x6a6c096404d1c7a3,
    0x3d857f91fc5f6232, 0x5c63557a89fa3a27, 0x3ee7bc50de6d3e04,
    0x2490253782ef8a57, 0xad4d59ff8fa5f4c,  0x1c01a62b4c726533,
    0x9ef66ba35cb5ab2e, 0x4c47d62317d71cdf, 0x2a40c557d5a3f2a0,
    0xf758338d53442abd, 0x1eaefcd073bcdd10, 0xb9833e0eb50e8fd7,
    0x8c966374151a67b6, 0x3edf7efe33cdfd7b, 0x5a2c4a1a310be558,
    0x69e43fcb628fbeb7, 0x99bd77ef54c90d36, 0xbab1c4f59f066e65,
    0x797fd05581e14764, 0x75de5603975e3ea9, 0x11ef41165111c73c,
    0xb7ddf0821a4031f5, 0x3eec9e62cf45d24c, 0xcda86289d51cd2c7,
    0x127483edee743fae, 0x4c62baba754703cf, 0xcd8d463bb2e9583a,
    0xaa38a80cc6c609f3, 0x5f97356ca50a5986, 0x220a1b6bc88caeb,
    0x20420bdd061f48c4, 0xb0e0c20838c35c57, 0xeddd78004259c3d2,
    0xb7b854be9806aa3b, 0xa2256b1fc3bd00fe, 0x3ea3e4597d23917d,
    0xd952cb1a7656852e, 0x89e052b4e095f921, 0x4be913d1c4f79a82,
    0x9d36a507aa427257, 0x69b7818b57dbbf8,  0x21102213d2cae2cd,
    0x27ec5888afbdb47e, 0x4f16a4e8c63e27f9, 0x250a3857af744546,
    0x3e97bbeec0fcabdd, 0x4b6c65f8ca7fd632, 0xa91fce0f23e1be61,
    0x5050c5a688052206, 0x9e2e3f1a6b85328f, 0xe09ec18b956f5fea,
    0x4000f41b1b5b25e7, 0x3fbea3faba569084, 0xe471347f40647f65,
    0xda7abaa5f3caf2c8, 0x189201a4940001f9, 0x29e183e37d7da328,
    0xd2ae28418f093e67, 0x8e5cb797039be80e, 0x41604277260a071d,
    0x11edac1b01696b0e, 0x6e27fc4394cebee3, 0xeeffd1b2b382a2c0,
    0x2443385d4c4e195,  0x2ee724f1ad94f68e, 0xa4491e380c5cf7b,
    0xeb48a728aaf18d2e,
};
static uint64_t LEARv2[256];

// Rolling2Bytes Mask
static uint32_t FING_GEAR_08KB_ls = 0xd9300353 << 1;
static uint32_t FING_GEAR_02KB_ls = 0xd9000353 << 1;
static uint32_t FING_GEAR_32KB_ls = 0xd9f00353 << 1;

static uint64_t FING_GEAR_08KB_ls_64 = 0x0000d93003530000 << 1;
static uint64_t FING_GEAR_02KB_ls_64 = 0x0000d90003530000 << 1;
static uint64_t FING_GEAR_32KB_ls_64 = 0x0000d9f003530000 << 1;
static uint64_t FING_GEAR_08KB_64 = 0x0000d93003530000;

static uint64_t FING_GEAR_02KB_64 = 0x0000d90003530000;
static uint64_t FING_GEAR_32KB_64 = 0x0000d9f003530000;

static uint32_t g_global_matrix[SymbolCount];
static uint32_t g_global_matrix_left[SymbolCount];
static uint32_t Mask_15 = 0xf9070353;
static uint32_t Mask_11 = 0xd9000353;
static uint64_t Mask_11_64 = 0x0000d90003530000;
static uint64_t Mask_15_64 = 0x0000f90703530000;

static uint32_t MinSize;
static uint32_t MinSize_divide_by_2;
static uint32_t MaxSize;

static int (*chunking) (unsigned char*p, int n);

// global variants

float totalTm = 0;

// 静态初始化函数
__attribute__((constructor))
static void initialize_global_variables() {
    unsigned char md5_digest[16];
    uint8_t seed[SeedLength];
    for (int i = 0; i < SymbolCount; i++) {

        for (int j = 0; j < SeedLength; j++) {
            seed[j] = i;
        }

        g_global_matrix[i] = 0;
        MD5(seed, SeedLength, md5_digest);
        memcpy(&(g_global_matrix[i]), md5_digest, 4);
        g_global_matrix_left[i] = g_global_matrix[i] << 1;
    }

    // 64 bit init
    for (int i = 0; i < SymbolCount; i++) {
        LEARv2[i] = GEARv2[i] << 1;
    }
}


static int normalized_chunking_64(unsigned char *p, int n) {
    uint64_t fingerprint = 0, digest;
    MinSize = 6 * 1024;
    int i = MinSize, Mid = 8 * 1024;

    // the minimal subChunk Size.
    if (n <= MinSize)  
        return n;

    if (n > MaxSize)
        n = MaxSize;
    else if (n < Mid)
        Mid = n;

    while (i < Mid) {
        fingerprint = (fingerprint << 1) + (GEARv2[p[i]]);

        if ((!(fingerprint & FING_GEAR_32KB_64))) {
            return i;
        }

        i++;
    }

    while (i < n) {
        fingerprint = (fingerprint << 1) + (GEARv2[p[i]]);

        if ((!(fingerprint & FING_GEAR_02KB_64))) {
            return i;
        }

        i++;
    }

    return n;
}

static int check_file_integrity(const char* file_path, Chunk* chunks, int chunkListSize, size_t totalProcessedBytes, const char* output_file_path) {
    // Check the total size of the file
    struct stat file_stat;
    if (stat(file_path, &file_stat) == 0) {
        if (totalProcessedBytes == file_stat.st_size) {
            printf("Data integrity check (size) passed: Processed bytes (%zu) match file size (%lld).\n", totalProcessedBytes, (long long)file_stat.st_size);

            // Reassemble the file from chunks and compare with the original file
            unsigned char *reassembledData = (unsigned char *)malloc(totalProcessedBytes);
            size_t reassembledOffset = 0;

            for (int i = 0; i < chunkListSize; i++) {
                memcpy(reassembledData + reassembledOffset, chunks[i].data, chunks[i].size);
                reassembledOffset += chunks[i].size;
            }

            // Read original file data
            FILE *originalFile = fopen(file_path, "rb");
            unsigned char *originalData = (unsigned char *)malloc(file_stat.st_size);
            fread(originalData, 1, file_stat.st_size, originalFile);
            fclose(originalFile);

            // Compare reassembled data with original data
            int result = memcmp(reassembledData, originalData, totalProcessedBytes);
            if (result == 0) {
                printf("Data integrity check (content) passed: Reassembled data matches original file content.\n");

                FILE *outputFile = fopen(output_file_path, "wb");
                if (outputFile) {
                    fwrite(reassembledData, 1, totalProcessedBytes, outputFile);
                    fclose(outputFile);
                    printf("Reassembled data written to %s for further inspection.\n", output_file_path);
                } else {
                    perror("Failed to open output file for writing");
                }
            } else {
                printf("Data integrity check (content) failed: Reassembled data does not match original file content.\n");

                // Write the reassembled data to the specified output file
                FILE *outputFile = fopen(output_file_path, "wb");
                if (outputFile) {
                    fwrite(reassembledData, 1, totalProcessedBytes, outputFile);
                    fclose(outputFile);
                    printf("Reassembled data written to %s for further inspection.\n", output_file_path);
                } else {
                    perror("Failed to open output file for writing");
                }
            }

            // Free allocated memory
            free(reassembledData);
            free(originalData);
            return result == 0;
        } else {
            printf("Data integrity check (size) failed: Processed bytes (%zu) do not match file size (%lld).\n", totalProcessedBytes, (long long)file_stat.st_size);
            return 0;
        }
    } else {
        perror("Failed to get file size for integrity check");
        return 0;
    }
}

static int normalized_chunking_2byes_64(unsigned char *p, int n) {
    uint64_t fingerprint = 0, digest;
    MinSize = 6 * 1024;
    int i = MinSize / 2, Mid = 8 * 1024;

    // the minimal subChunk Size.
    if (n <= MinSize) 
        return n;

    if (n > MaxSize)
        n = MaxSize;
    else if (n < Mid)
        Mid = n;

    while (i < Mid / 2) {
        int a = i * 2;
        fingerprint = (fingerprint << 2) + (LEARv2[p[a]]);

        if ((!(fingerprint & FING_GEAR_32KB_ls_64))) {
            return a;
        }

        fingerprint += GEARv2 [p[a + 1]];  

        if ((!(fingerprint & FING_GEAR_32KB_64))) {
            return a + 1;
        }

        i++;
    }

    while (i < n / 2) {
        int a = i * 2;
        fingerprint = (fingerprint << 2) + (LEARv2[p[a]]);

        if ((!(fingerprint & FING_GEAR_02KB_ls_64))) {
            return a;
        }

        fingerprint += GEARv2[p[a + 1]];

        if ((!(fingerprint & FING_GEAR_02KB_64))) {
            return a + 1;
        }

        i++;
    }

    return n;
}

static int rolling_data_2byes_64(unsigned char *p, int n) {
    uint64_t fingerprint = 0, digest;
    int i = MinSize_divide_by_2;

    // the minimal subChunk Size.
    if (n <= MinSize) 
        return n;

    if (n > MaxSize) n = MaxSize;

    while (i < n / 2) {
        int a = i * 2;
        fingerprint = (fingerprint << 2) + (LEARv2[p[a]]);

        if ((!(fingerprint & FING_GEAR_08KB_ls_64))) {
            return a;
        }

        fingerprint += GEARv2[p[a + 1]];

        if ((!(fingerprint & FING_GEAR_08KB_64))) {
            return a + 1;
        }

        i++;
    }

    return n;
}

static int cdc_origin_64(unsigned char *p, int n) {
    uint64_t fingerprint = 0, digest;
    int i = MinSize;
    // return n;
    // the minimal subChunk Size.
    if (n <= MinSize)  
        return n;

    if (n > MaxSize) n = MaxSize;

    while (i < n) {
        fingerprint = (fingerprint << 1) + (GEARv2[p[i]]);
        if ((!(fingerprint & FING_GEAR_08KB_64))) {
            return i;
        }
        i++;
    }

    return n;
}


Chunk *process_file_chunks(const char *file_path, size_t _min_size, size_t _max_size, size_t _avg_size, int *chunkCount) {
    // Set default values if not provided
    if (_avg_size == 0) {
        _avg_size = DEFAULT_AVG_SIZE;
    }

    MinSize = _min_size == 0 ? _avg_size / 4 : _min_size;
    MaxSize = _max_size == 0 ? _avg_size * 4 : _max_size;

    // printf("MinSize=%d\n", MinSize);
    // printf("MaxSize=%d\n", MaxSize);

    // Initialize necessary variables
    FILE *file;
    size_t readStatus = 0;

    int chunk_num = 0;
    unsigned char *fileCache = (unsigned char *)malloc(CacheSize);
    memset(fileCache, 0, CacheSize);

    int offset = 0, chunkLength = 0, readFlag = 0;
    size_t globalOffset = 0; // Track the global offset
    struct timeval tmStart, tmEnd;
    long totalTm = 0;

    // Initialize chunking method
    switch (USE_CHUNKING_METHOD) {
        case ORIGIN_CDC:
            chunking = cdc_origin_64;
            break;
        case ROLLING_2Bytes:
            chunking = rolling_data_2byes_64;
            break;
        case NORMALIZED_CDC:
            chunking = normalized_chunking_64;
            break;
        case NORMALIZED_2Bytes:
            chunking = normalized_chunking_2byes_64;
            break;
        default:
            fprintf(stderr, "No implement other chunking method\n");
            return NULL;
    }

    // Open the file
    file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Fail to open file");
        exit(-1);
    }

    readStatus = fread(fileCache, 1, CacheSize, file);
    int end = readStatus;

    // Variable to track total bytes processed
    size_t totalProcessedBytes = 0;

    // List to store chunks
    Chunk *chunks = NULL;
    int chunkListSize = 0;

    // Process the file in chunks
    for (;;) {
        chunk_num += 1;
        gettimeofday(&tmStart, NULL);

        // Perform chunking
        chunkLength = chunking(fileCache + offset, readStatus - offset);

        // Store chunk information
        Chunk chunk;
        chunk.id = chunk_num;
        chunk.size = chunkLength;
        chunk.offset = globalOffset + offset;
        chunk.data = (unsigned char *)malloc(chunkLength);
        memcpy(chunk.data, fileCache + offset, chunkLength);

        // Add chunk to the list
        chunkListSize++;
        chunks = (Chunk *)realloc(chunks, chunkListSize * sizeof(Chunk));
        chunks[chunkListSize - 1] = chunk;

        // printf("%d-th chunk, length=%d, offset=%zu\n", chunk_num, chunkLength, globalOffset + offset);

        gettimeofday(&tmEnd, NULL);
        totalTm += ((tmEnd.tv_sec - tmStart.tv_sec) * 1000000 + (tmEnd.tv_usec - tmStart.tv_usec)); // microseconds

        offset += chunkLength;
        totalProcessedBytes += chunkLength;

        if (readStatus - offset < MaxSize) {
            memcpy(fileCache, fileCache + offset, readStatus - offset);

            // Continuing read file
            int readStatus_tmp = fread(fileCache + readStatus - offset, 1, offset, file);

            // Recording the end point
            end = readStatus - offset + readStatus_tmp;

            // readStatus is that current how many bytes we have read?
            readStatus = end;

            if (readStatus_tmp < offset) {
                // all the files are read
                readFlag = 1;
            }

            offset = 0;
        }

        globalOffset += chunkLength;

        if (offset >= end && readFlag == 1)
            break;
    }

    // printf("totalTime is %f s\n", totalTm / 1000000.0);
    // printf("chunknum is %d\n", chunk_num);

    // Close the file
    fclose(file);

    // Check data integrity
    // if (!check_file_integrity(file_path, chunks, chunkListSize, totalProcessedBytes, "output_reassembled.bin")) {
    //     // Integrity check failed, free allocated memory
    //     for (int i = 0; i < chunkListSize; i++) {
    //         free(chunks[i].data);
    //     }
    //     free(chunks);
    //     free(fileCache);
    //     return NULL;
    // }

    *chunkCount = chunkListSize;
    free(fileCache);
    return chunks;
}
import random
import numpy as np
import xxhash
import array
from collections import deque
import hnswlib
random.seed(47)
GEARv2 = [
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
]

# import mmh3

# def murmurhash64(data):
#     hash_value = mmh3.hash64(data)
#     return hash_value
class BufferedHNSW:
    def __init__(self, max_size, vector_dim, max_elements, hnsw_space='l2'):
        self.vector_dim = vector_dim
        # 初始化 hnswlib 索引
        self.max_elements = max_elements
        self.hnsw_index = hnswlib.Index(space=hnsw_space, dim=vector_dim)
        self.hnsw_index.init_index(max_elements=max_elements, ef_construction=200, M=5)

    def add(self, element_id, vector):
        # 直接将新元素添加到 HNSW 索引中
        if self.hnsw_index.element_count < self.max_elements:
            self.hnsw_index.add_items(vector[np.newaxis, :], [element_id])

    def knn_query(self, query_vector, k=1):
        # 执行 k 最近邻查询
        return self.hnsw_index.knn_query(query_vector, k=k)
# class BufferedHNSW:
#     def __init__(self, max_size, vector_dim, max_elements, hnsw_space='l2'):
#         self.max_size = max_size
#         self.vector_dim = vector_dim
#         self.buffer = deque(maxlen=max_size)
#         self.vectors = np.zeros((max_size, vector_dim), dtype=np.float32)
#         self.ids = deque(maxlen=max_size)
#         self.current_size = 0
        
#         # 初始化 hnswlib 索引
#         self.max_elements = max_elements
#         self.hnsw_index = hnswlib.Index(space=hnsw_space, dim=vector_dim)
#         self.hnsw_index.init_index(max_elements=max_elements, ef_construction=200, M=5)
        
#     def add(self, element_id, vector):
#         if len(self.buffer) == self.max_size:
#             # 将缓冲区中的数据添加到 HNSW 索引中
#             self.flush()

#         # 添加新元素到缓冲区
#         self.buffer.append((element_id, vector))
#         self.ids.append(element_id)
#         self.vectors[self.current_size] = vector
#         self.current_size += 1
        
#     def flush(self):
#         if self.current_size == 0:
#             return
        
#         # 将缓冲区中的数据添加到 HNSW 索引中
#         ids = list(self.ids)
#         vectors = self.vectors[:self.current_size]
#         if self.hnsw_index.element_count + len(ids) < self.max_elements:
#             self.hnsw_index.add_items(vectors, ids)
#         # 清空缓冲区
#         self.buffer.clear()
#         self.ids.clear()
#         self.current_size = 0
        
#     def knn_query(self, query_vector, k=1):
#         return self.hnsw_index.knn_query(query_vector, k=k)

class MyDICE:
    def __init__(self, DIMENSION, BUFFER_SIZE, THRESHOLD, MAX_ELEMENTS):
        self.mask = 0x00000007f0000000
        self.dim = DIMENSION
        self.buffer_size = BUFFER_SIZE
        self.bhnsw = BufferedHNSW(self.buffer_size, DIMENSION, MAX_ELEMENTS)
        self.threshold = THRESHOLD
        self.byte2vec = np.float32(np.random.random((256, self.dim)))
        self.cur_vector = None
        
        self.count_hnsw_found = 0
        self.count_bhnsw_found = 0
        self.not_found = 0
        
    def process_chunk(self, chunk: bytes) -> np.ndarray:
        # 创建一个大小为 256 的数组，用于存储每个字节的计数
        byte_counts = array.array('I', [0] * 256)
        
        # 统计每个字节的出现次数
        for byte in chunk:
            byte_counts[byte] += 1
        
        # 创建一个形状为 (dim,) 的零向量，用于累加结果
        result_vector = np.zeros(self.dim, dtype=np.float32)
        
        # 遍历每个字节值，累加向量
        for byte_value in range(256):
            count = byte_counts[byte_value]
            if count > 0:
                result_vector += self.byte2vec[byte_value] * count

        # 除以 chunk 的长度
        result_vector /= len(chunk)

        return result_vector
    
    def request(self, chunk:bytes):
        self.cur_vector = self.process_chunk(chunk)
        min_dist = float('inf')
        ret_id = -1
        found_by_bhnsw = False
        
        # 1. 遍历缓冲区，计算欧几里得距离，记录最小距离及对应的 ID
        # if self.bhnsw.current_size > 0:
        #     for i in range(self.bhnsw.current_size):
        #         vector = self.bhnsw.vectors[i]
        #         element_id = self.bhnsw.ids[i]
        #         dist = np.linalg.norm(self.cur_vector - vector)
        #         if dist < min_dist:
        #             min_dist = dist
        #             ret_id = element_id
        #             found_by_bhnsw = True
        
        # 2. 使用 hnsw_index 查询最近的节点
        if self.bhnsw.hnsw_index.element_count > 0:
            labels, distances = self.bhnsw.knn_query(self.cur_vector, k=1)
            hnsw_dist = np.sqrt(distances[0][0])
            hnsw_id = labels[0][0]
            if hnsw_dist < min_dist:
                min_dist = hnsw_dist
                ret_id = hnsw_id
                found_by_bhnsw = False
        
        # 3. 对比最小距离与阈值
        # print(f'min_dist: {min_dist}')
        if min_dist < self.threshold:
            if found_by_bhnsw:
                self.count_bhnsw_found += 1
            else:
                self.count_hnsw_found += 1
            return ret_id
        else:
            self.not_found += 1
            return -1
        
        # self.bhnsw = BufferedHNSW(self.buffer_size, DIMENSION, MAX_ELEMENTS)
        # 1. 如果bhnsw中的buffer长度>0，对buffer中依次遍历，计算当前cur_vector与buffer中每个元素的vector的欧几里得距离，然后记录最小的欧几里得距离以及对应的id
        # 2. 如果bhnsw中的hnsw_index中元素个数>0，则使用hnsw_index来查询最近的节点，返回节点的id以及距离，注意这里的距离并没有开方，因此我们要对结果开方
        # 3. 对比bhnsw返回的两种结果，记录最短距离以及对应的id
        # 4. 如果最小距离 < self.threshold，则返回对应的id，否则返回-1
        
    
    
    def insert(self, label):
        self.bhnsw.add(label, self.cur_vector)
        
    def stats(self):
        total_finds = self.count_hnsw_found + self.count_bhnsw_found
        if total_finds == 0:
            return "No elements found yet."
        ratio_hnsw = self.count_hnsw_found / total_finds
        ratio_bhnsw = self.count_bhnsw_found / total_finds
        return {
            "hnsw_found": self.count_hnsw_found,
            "bhnsw_found": self.count_bhnsw_found,
            "not_found": self.not_found,
            "ratio_hnsw": ratio_hnsw,
            "ratio_bhnsw": ratio_bhnsw
        }

# def test_my_dice():
#     # 参数设置
#     DIMENSION = 128
#     BUFFER_SIZE = 3
#     THRESHOLD = 0.5
#     MAX_ELEMENTS = 100

#     # 创建 MyDICE 实例
#     my_dice = MyDICE(DIMENSION, BUFFER_SIZE, THRESHOLD, MAX_ELEMENTS)

#     # 创建一些随机的字节块
#     chunk1 = bytes(np.random.randint(0, 256, 100))
#     chunk2 = bytes(np.random.randint(0, 256, 100))
#     chunk3 = bytes(np.random.randint(0, 256, 100))
#     chunk4 = bytes(np.random.randint(0, 256, 100))
#     chunk5 = bytes(np.random.randint(0, 256, 100))

#     # 添加字节块到 MyDICE 实例中
#     my_dice.bhnsw.add(1, my_dice.process_chunk(chunk1))
#     my_dice.bhnsw.add(2, my_dice.process_chunk(chunk2))
#     my_dice.bhnsw.add(3, my_dice.process_chunk(chunk3))
#     my_dice.bhnsw.add(4, my_dice.process_chunk(chunk4))

#     # 测试 request 方法
#     # 1. 使用一个相似的 chunk，期望返回一个 ID
#     similar_chunk = chunk1
#     result = my_dice.request(similar_chunk)
#     print(f"Request result for similar chunk: {result}")
    
#     # 2. 使用一个不同的 chunk，期望返回 -1
#     different_chunk = chunk5
#     result = my_dice.request(different_chunk)
#     print(f"Request result for different chunk: {result}")

# # 运行测试
# test_my_dice()
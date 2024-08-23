import sys
sys.path.append('/home/devuser/codebase/deepsketch-fast2022/odess/py')
import os
from fastcdc import fastcdc
import xxhash
import time
import xdelta3
# import math
from tqdm import tqdm

from deep import DeepSketch

def delta_encoding(base_chunk_data, chunk_data):
    try:
        diff_data = xdelta3.encode(base_chunk_data, chunk_data)
        return len(diff_data)
    except BaseException:
        pass
    return -1
    

if __name__ == "__main__":
    if len(sys.argv) != 6:
        print("Usage: python script.py [input_file] [DIMENSION] [BUFFER_SIZE] [AVERAGE_CHUNK_SIZE] [THRESHOLD]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    filesize = os.path.getsize(input_file)
    AVERAGE_CHUNK_SIZE = int(sys.argv[2])
    DIMENSION = int(sys.argv[3])
    BUFFER_SIZE = int(sys.argv[4])
    THRESHOLD = float(sys.argv[5])
    
    # statistics
    error_chunks = 0
    total = 0
    total_non_duplicated = 0
    delta_chunks = 0
    no_delta_chunks = 0
    time_cost = 0
    
    # Init
    dedup = {}
    
    cdc_results = list(fastcdc(input_file, avg_size=AVERAGE_CHUNK_SIZE, fat=True))
    total_chunks = len(cdc_results)
    dice_ins = DeepSketch(DIMENSION, BUFFER_SIZE, THRESHOLD, int(total_chunks * 0.8))
    
    # Start
    dedup_start = time.perf_counter()
    
    chunks = []
    for _idx, chunk in tqdm(enumerate(cdc_results), total=total_chunks):
        hash_value = xxhash.xxh64(chunk.data, seed=0).intdigest()
        if hash_value in dedup:
            continue
        dedup[hash_value] = 1
        chunks.append(chunk)
    merged_data = b''.join(chunk.data for chunk in chunks)
    fixed_size = 4096  # 例如，每个块1024字节
    fixed_length_chunks = [merged_data[i:i+fixed_size] for i in range(0, len(merged_data), fixed_size)]
    # for _idx, chunk in enumerate(cdc_results):

            
    print(f"len chunks: {len(fixed_length_chunks)}")
    for chunk in tqdm(fixed_length_chunks, total=len(fixed_length_chunks)):
        if len(chunk) < 4096:
            print(f'len of chunk: {len(chunk)}')
            break
        
        dcomp_lsh_ref = dice_ins.request(chunk)
        
    print("******************** DICE - Begin ********************\n")
    print(f"Trace: {input_file}")
    print(f"Avg chunk size: {AVERAGE_CHUNK_SIZE}")
    print(f"Total time: {time.perf_counter() - dedup_start}")
    print("********************* DICE - End *********************")

import sys
sys.path.append('/home/devuser/codebase/deepsketch-fast2022/odess/py')
import os
from fastcdc import fastcdc
import xxhash
import time
import xdelta3
# import math
from tqdm import tqdm
from finesse import MyFinesse

def delta_encoding(base_chunk_data, chunk_data):
    try:
        diff_data = xdelta3.encode(base_chunk_data, chunk_data)
        return len(diff_data)
    except BaseException:
        pass
    return -1
    

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: python script.py [input_file] [SF_NUM] [FEATURE_NUM] [average_chunk_size]")
        sys.exit(1)
    
    input_file = sys.argv[1]
    filesize = os.path.getsize(input_file)
    SF_NUM = int(sys.argv[2])
    FEATURE_NUM = int(sys.argv[3])
    average_chunk_size = int(sys.argv[4])
    
    # statistics
    error_chunks = 0
    total = 0
    total_non_duplicated = 0
    delta_chunks = 0
    no_delta_chunks = 0
    time_cost = 0
    total_time_cost = 0
    
    # Init
    dedup = {}
    finesse_ins = MyFinesse(SF_NUM, FEATURE_NUM)
    
    # Start
    dedup_start = time.perf_counter()
    
    cdc_results = list(fastcdc(input_file, avg_size=average_chunk_size, fat=True))
    
    # for _idx, chunk in tqdm(enumerate(cdc_results), total=len(cdc_results)):
    for _idx, chunk in enumerate(cdc_results):
        hash_value = xxhash.xxh64(chunk.data, seed=0).intdigest()
        if hash_value in dedup:
            continue
        dedup[hash_value] = 1
        total_non_duplicated += chunk.length
        
        dcomp_lsh = sys.maxsize
        
        start = time.perf_counter()
        dcomp_lsh_ref = finesse_ins.request(chunk.data)
        time_cost += (time.perf_counter() - start)
        
        if dcomp_lsh_ref != -1:
            dcomp_lsh = delta_encoding(cdc_results[dcomp_lsh_ref].data, chunk.data)
            if dcomp_lsh > chunk.length:
                error_chunks += 1
        if dcomp_lsh < chunk.length:
            total += dcomp_lsh
            delta_chunks +=1
        else:
            total += chunk.length
            no_delta_chunks +=1
            finesse_ins.insert(_idx)
        
        
    print("****************** FINESSE - Begin ******************\n")
    print(f"Trace: {input_file}")
    print(f"Avg chunk size: {average_chunk_size}")
    print(f"Request time: {time_cost}")
    print(f"Total time: {time.perf_counter() - dedup_start}")
    print(f"LSH: Odess, SF = {SF_NUM}, feature = {FEATURE_NUM}")
    print(f"Total      Chunks: {len(cdc_results)}")
    print(f"Duplicated Chunks: {len(cdc_results) - delta_chunks - no_delta_chunks}")
    print(f"Base       Chunks: {no_delta_chunks}")
    print(f"Similar    Chunks: {delta_chunks}");
    print(f"Error Chunks: {error_chunks}, DCE={1.0 * error_chunks / (delta_chunks + no_delta_chunks):.5f}")
    print(f"Origin size: {total_non_duplicated}")
    print(f"Final DCR size: {total} ({1.0 * total_non_duplicated / total:.2f})")
    print(f"File size: {filesize}")
    print(f"Final DRR size: {total} ({1.0 * filesize / total:.2f})")
    print("******************* FINESSE - End *******************")

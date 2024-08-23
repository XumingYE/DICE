#!/bin/bash

# 设置avg_size值列表，单位为K，因此1024等于1M
sizes=(32768 16384 8192 4096 2048)
datasets=("cpu") # "sql" "linux" "docker" "img" "nlp" "stack" "cpu"

dimension=50

# 使用关联数组设置每个数据集对应的threshold
declare -A thresholds
thresholds[sql]=0.216778031390 # 0.216778031390
thresholds[linux]=0.227499
thresholds[docker]=0.6721059899
thresholds[img]=0.483916312 # 0.013
thresholds[nlp]=0.020966328
thresholds[stack]=0.07680139
thresholds[cpu]=0.010659757004424



# 遍历dataset列表
for dataset in "${datasets[@]}"
do
    # DICE
    for size in "${sizes[@]}"
    do
        current_threshold=${thresholds[$dataset]}
        # 执行python命令,并将输出重定向到带有size后缀的日志文件
        ./dice_compv3 \
        /home/devuser/codebase/data/val_${dataset}.tar \
        ${size} \
        64 \
        128 \
        ${current_threshold}

        #./odess_comp /home/devuser/codebase/data/val_${dataset}.tar 32 3 12 ${size}

        #./finesse_comp /home/devuser/codebase/data/val_${dataset}.tar 32 3 12 ${size}
    done
done



# thresholds[sql]=0.156778031390 # 0.216778031390
# thresholds[linux]=0.315972040
# thresholds[docker]=0.6721059899
# thresholds[img]=0.046 # 0.013
# thresholds[nlp]=0.0291199
# thresholds[stack]=0.106668604
# thresholds[cpu]=0.0148052180617
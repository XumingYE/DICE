#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <unordered_set>
#include <vector>
#include <condition_variable>
#include <optional>
#include <cmath>
#include <stdexcept>

// 线程安全队列模板
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    bool closed_;

public:
    ThreadSafeQueue() : closed_(false) {}
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_) {
                throw std::runtime_error("Push to closed queue");
            }
            queue_.push(std::move(value));
        }
        cond_.notify_one();
    }

    // 阻塞获取元素，返回optional
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [&] { return !queue_.empty() || closed_; });
        if (!queue_.empty()) {
            T value = std::move(queue_.front());
            queue_.pop();
            return value;
        }
        return std::nullopt; // 队列关闭且为空
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        cond_.notify_all();
    }

    bool is_closed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_;
    }
};

// 数据块结构，可以根据实际情况修改
struct DataBlock {
    int id;
    std::string data;
    // 其他必要的字段
};

// 处理结果结构
struct ProcessedData {
    DataBlock block;
    size_t hash;
    std::vector<float> feature;
};

// 计算哈希值（示例实现）
size_t computeHash(const std::string& data) {
    return std::hash<std::string>{}(data);
}

// 计算特征（示例实现）
std::vector<float> computeFeature(const std::string& data) {
    // 实际特征计算逻辑
    return {static_cast<float>(data.length())};
}

// Pipeline类
class Pipeline {
private:
    ThreadSafeQueue<DataBlock> inputQueue_;
    ThreadSafeQueue<ProcessedData> hashQueue_;
    ThreadSafeQueue<ProcessedData> featureQueue_;
    ThreadSafeQueue<ProcessedData> outputQueue_;

    // 存储已见过的哈希和特征
    std::unordered_set<size_t> hashSet_;
    std::vector<std::vector<float>> featureList_;
    std::mutex hashMutex_;
    std::mutex featureMutex_;

    // 线程
    std::thread hashThread_;
    std::thread featureThread_;
    std::thread processThread_;

    void hashStage() {
        while (true) {
            auto optBlock = inputQueue_.pop();
            if (!optBlock.has_value()) {
                // 输入队列已关闭，关闭下一个阶段的队列并退出
                hashQueue_.close();
                break;
            }
            DataBlock block = optBlock.value();
            size_t hash = computeHash(block.data);

            {
                std::lock_guard<std::mutex> lock(hashMutex_);
                if (hashSet_.find(hash) != hashSet_.end()) {
                    // 重复，跳过
                    continue;
                }
                // 未重复，暂存hash
                hashSet_.insert(hash);
            }

            ProcessedData pdata;
            pdata.block = block;
            pdata.hash = hash;
            hashQueue_.push(pdata);
        }
    }

    void featureStage() {
        while (true) {
            auto optPData = hashQueue_.pop();
            if (!optPData.has_value()) {
                // 哈希阶段队列已关闭，关闭下一个阶段的队列并退出
                featureQueue_.close();
                break;
            }
            ProcessedData pdata = optPData.value();
            std::vector<float> feature = computeFeature(pdata.block.data);

            bool isSimilar = false;
            {
                std::lock_guard<std::mutex> lock(featureMutex_);
                for (const auto& existingFeature : featureList_) {
                    // 简单相似性判断，例如欧氏距离小于阈值
                    float distance = 0.0f;
                    for (size_t i = 0; i < feature.size(); ++i) {
                        distance += (feature[i] - existingFeature[i]) * (feature[i] - existingFeature[i]);
                    }
                    if (std::sqrt(distance) < 0.1f) { // 阈值0.1根据实际情况调整
                        isSimilar = true;
                        break;
                    }
                }
                if (!isSimilar) {
                    featureList_.push_back(feature);
                }
            }

            if (!isSimilar) {
                pdata.feature = feature;
                featureQueue_.push(pdata);
            }
            // 如果相似，则不做任何操作
        }
    }

    void processStage() {
        while (true) {
            auto optPData = featureQueue_.pop();
            if (!optPData.has_value()) {
                // 特征阶段队列已关闭，关闭输出队列（如果有）
                outputQueue_.close();
                break;
            }
            ProcessedData pdata = optPData.value();
            // 处理非重复的数据块
            // 例如，保存到数据库或进一步处理
            std::cout << "Processing DataBlock ID: " << pdata.block.id << std::endl;
        }
    }

public:
    Pipeline() {
        // 启动各个阶段的线程
        hashThread_ = std::thread(&Pipeline::hashStage, this);
        featureThread_ = std::thread(&Pipeline::featureStage, this);
        processThread_ = std::thread(&Pipeline::processStage, this);
    }

    ~Pipeline() {
        // 关闭输入队列，通知 hashStage 不再有新数据
        inputQueue_.close();

        // 等待线程结束
        if (hashThread_.joinable()) hashThread_.join();
        if (featureThread_.joinable()) featureThread_.join();
        if (processThread_.joinable()) processThread_.join();
    }

    void addDataBlock(const DataBlock& block) {
        inputQueue_.push(block);
    }

    // 可以添加其他必要的方法，例如等待处理完成
};


int main() {
    Pipeline pipeline;

    // 模拟数据块输入
    for(int i = 0; i < 100; ++i) {
        DataBlock block;
        block.id = i;
        block.data = "SampleData_" + std::to_string(i % 10); // 有重复数据
        pipeline.addDataBlock(block);
    }

    // 等待一段时间让Pipeline处理（实际应用中应有更好的同步机制）
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}

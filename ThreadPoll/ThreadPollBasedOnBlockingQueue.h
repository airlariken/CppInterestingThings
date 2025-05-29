//
// Created by orange on 4/8/25.
//
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>

template <typename T>
class BlockingQueue {
public:
    BlockingQueue(bool nonblock = false) : nonblock_(nonblock) {}
    // 入队操作
    void Push(const T &value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        not_empty_.notify_one();
    }

    // 出队操作
    bool Pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this] { return !queue_.empty() || nonblock_; });
        if (queue_.empty())
            return false;

        value = queue_.front();
        queue_.pop();
        return true;
    }

    // 解除阻塞在当前队列的线程
    void Cancel() {
        std::lock_guard<std::mutex> lock(mutex_);
        nonblock_ = true;
        not_empty_.notify_all();
    }
private:
    bool nonblock_;
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_empty_;
};

template <typename T>
class BlockingQueuePro {
public:
    BlockingQueuePro(bool nonblock = false) : nonblock_(nonblock) {}
    // 入队操作
    void Push(const T &value) {
        std::lock_guard<std::mutex> lock(producer_mutex_);
        producer_queue_.push(value);
        not_empty_.notify_one();
    }

    // 出队操作
    bool Pop(T& value) {
        std::unique_lock<std::mutex> lock(consumer_mutex_);
        if (consumer_queue_.empty() && SwapQueue_() == 0) {
            return false;
        }
        value = consumer_queue_.front();
        consumer_queue_.pop();
        return true;
    }

    // 解除阻塞在当前队列的线程
    void Cancel() {
        std::lock_guard<std::mutex> lock(producer_mutex_);
        nonblock_ = true;
        not_empty_.notify_all();
    }
private:

    // 当消费者队列为空时，交换生产者和消费者队列
    int SwapQueue_() {
        std::unique_lock<std::mutex> lock(producer_mutex_);
        not_empty_.wait(lock, [this] { return !producer_queue_.empty() || nonblock_; });
        std::swap(producer_queue_, consumer_queue_);
        return consumer_queue_.size();
    }

    bool nonblock_;
    std::queue<T> producer_queue_;
    std::queue<T> consumer_queue_;
    std::mutex producer_mutex_;
    std::mutex consumer_mutex_;
    std::condition_variable not_empty_;
};

class ThreadPool {
public:
    // 构造函数，初始化线程池
    explicit ThreadPool(int num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this] { Worker(); });
        }
    }

    // 析构函数，停止线程池
    ~ThreadPool() {
        task_queue_.Cancel();
        for (auto &worker : workers_) {
            if (worker.joinable())
                worker.join();
        }
    }

    // 发布任务到线程池
    // P.S. F 表示一个可调用对象(函数、lambda 表达式、函数对象等)，Args 表示函数对象的参数类型
    // Args... 是一个可变参数模板，表示可以接受任意数量的参数
    template<typename F, typename... Args>
    void Post(F &&f, Args &&...args) {
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        task_queue_.Push(task);
    }

private:
    // 工作线程入口函数
    void Worker() {
        while (true) {
            std::function<void()> task;
            if (!task_queue_.Pop(task))
                break;
            task();
        }
    }

    BlockingQueue<std::function<void()>> task_queue_;   // 任务队列
    std::vector<std::thread> workers_;                  // 工作线程
};


//
// Created by orange on 4/1/25.
//
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    // 构造函数：创建指定数量的工作线程
    ThreadPool(size_t threads);
    // 析构函数：停止所有线程并回收资源
    ~ThreadPool();

    // 添加任务到线程池，返回一个 std::future 用于获取任务返回值
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    // 存储工作线程
    std::vector<std::thread> workers;
    // 任务队列
    std::queue<std::function<void()>> tasks;

    // 同步所需的互斥量和条件变量
    std::mutex queue_mutex;
    std::condition_variable condition;
    // 停止标志
    bool stop;
};

// 构造函数：创建线程并启动无限循环等待任务
ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(
            [this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        // 等待任务或者停止信号
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });
                        if (this->stop && this->tasks.empty())
                            return;
                        // 获取任务并出队
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    // 执行任务
                    task();
                }
            }
        );
    }
}

// 添加任务到线程池，返回 std::future 用于获取任务结果
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    // 封装任务
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        // 如果线程池已经停止，则抛出异常
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

// 析构函数：停止所有线程并等待线程退出
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
        worker.join();
}

// 测试示例
int main() {
    ThreadPool pool(4); // 创建一个包含4个线程的线程池
    std::vector<std::future<int>> results;

    // 提交8个任务到线程池
    for (int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool.enqueue([i] {
                std::cout << "正在处理任务 " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::cout << "完成任务 " << i << std::endl;
                return i * i;
            })
        );
    }

    // 获取所有任务的返回值
    for (auto && result : results)
        std::cout << "结果: " << result.get() << std::endl;

    return 0;
}

//
// Created by 橙子味 on 2025/1/8.
//
#include <iostream>
#include <future>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <random>
std::queue<std::packaged_task<int()>> tasks;
std::mutex mtx;
std::condition_variable cv;

void worker() {
    while (true) {
        std::packaged_task<int()> task;

        // 获取任务
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return !tasks.empty(); });
            task = std::move(tasks.front());
            tasks.pop();
        }

        // 执行任务
        task();
    }
}

int main() {
    // 启动工作线程
    std::thread t(worker);
    std::vector<std::future<int>> results;
    std::mt19937 engine(std::random_device{}()); // 使用随机设备初始化种子

    // 定义分布（整数范围 [1, 100]）
    std::uniform_int_distribution<int> dist(1, 3);
    for (int i = 0; i < 10; i++) {

        std::packaged_task<int()> task([&dist, &engine] {
            std::cout << "Task is running in thread." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(dist(engine)));
            return 42;
        });

        std::future<int> result = task.get_future();

        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push(std::move(task));
        }
        cv.notify_one();
        results.emplace_back(std::move(result));


    }
    // 创建任务并将其推入队列

    for (auto & result : results) {
        // 获取任务结果
        std::cout << "Task result: " << result.get() << std::endl;
    }
    // t.detach(); // 让线程独立运行，程序结束时退出
    t.join();

    return 0;
}

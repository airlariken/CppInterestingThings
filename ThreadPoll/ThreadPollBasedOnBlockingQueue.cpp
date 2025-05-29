//
// Created by orange on 4/8/25.
//
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

// 包含 BlockingQueue 和 ThreadPool 的定义
#include "ThreadPollBasedOnBlockingQueue.h"

// 全局计数器，用于统计任务完成的数量
std::atomic<int> task_counter{0};

// 任务函数
void Task(int id) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟任务执行时间
    std::cout << "Task " << id << " executed by thread " << std::this_thread::get_id() << std::endl;
    task_counter++; // 任务完成，计数器加一
}

// 生产者线程函数
void Producer(ThreadPool& pool, int producer_id, int num_tasks) {
    for (int i = 0; i < num_tasks; ++i) {
        int task_id = producer_id * 1000 + i; // 生成唯一的任务ID
        pool.Post(Task, task_id); // 提交任务到线程池
        std::cout << "Producer " << producer_id << " posted task " << task_id << std::endl;
    }
}

int main() {
    const int num_producers = 4;  // 生产者线程数量
    const int num_tasks_per_producer = 10; // 每个生产者提交的任务数量
    const int num_threads_in_pool = 2; // 线程池中的工作线程数量

    ThreadPool pool(num_threads_in_pool); // 创建线程池

    std::vector<std::thread> producers; // 生产者线程集合

    // 启动生产者线程
    for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back(Producer, std::ref(pool), i, num_tasks_per_producer);
    }

    // 等待所有生产者线程完成
    for (auto& producer : producers) {
        producer.join();
    }

    // 等待所有任务完成
    while (task_counter < num_producers * num_tasks_per_producer) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "All tasks completed. Total tasks executed: " << task_counter << std::endl;

    return 0;
}
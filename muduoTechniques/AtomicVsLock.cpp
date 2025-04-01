//
// Created by 橙子味 on 2025/3/28.
//
#include <iostream>
#include <thread>
#include <unistd.h>
#include <atomic>
#include <sys/time.h>
#include <mutex>
#include <functional>
const int iter_num = 100000;
const int thread_num = 3;

int g_count = 0;

std::atomic<bool> atomic_lock(false);
std::mutex mtx;

void unsafe_worker() {
    for (int i = 0; i < iter_num; ++i) {
        ++g_count;
    }
}

/**
  * 可以用 atomic<int> 轻易地实现线程安全的计数器,
  * 这里用 atomic 实现自旋锁, 主要是为了和 lock 做对比
  */
void atomic_worker() {
    for (int i = 0; i < iter_num; ++i) {
        bool expected = false;
        // 自旋锁
        do {
            expected = false;
        } while (!atomic_lock.compare_exchange_weak(expected, true));
        ++g_count;
        atomic_lock.store(false);
    }
}

void lock_worker() {
    for (int i = 0; i < iter_num; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        ++g_count;
    }
}

void run(std::function<void()> worker) {
    g_count = 0;
    std::thread threads[thread_num];
    for (int i = 0; i < thread_num; ++i) {
        threads[i] = std::thread(worker);
    }
    for (int i = 0; i < thread_num; ++i) {
        threads[i].join();
    }
}

int main() {
    timeval start_time = (struct timeval) {0};
    timeval end_time = (struct timeval) {0};
    long interval = 0;

    int expected = iter_num * thread_num;

    run(unsafe_worker);
    std::cout << "g_count = " << g_count << ", expected : " << expected << std::endl;

    gettimeofday(&start_time, NULL);
    run(atomic_worker);
    gettimeofday(&end_time, NULL);
    interval = ((end_time.tv_sec - start_time.tv_sec) * 1000 * 1000 + (end_time.tv_usec - start_time.tv_usec));
    std::cout << "g_count = " << g_count << ", expected : " << expected << ", interval : " << interval << std::endl;

    gettimeofday(&start_time, NULL);
    run(lock_worker);
    gettimeofday(&end_time, NULL);
    interval = ((end_time.tv_sec - start_time.tv_sec) * 1000 * 1000 + (end_time.tv_usec - start_time.tv_usec));
    std::cout << "lock_g_count = " << g_count << ", expected : " << expected << ", interval : " << interval << std::endl;

    return 0;
}
// gpt: 自旋锁性能非常依赖线程争抢强度，如果 CPU 核少、调度差，会特别慢。
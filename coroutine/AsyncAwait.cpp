//
// Created by orange on 5/15/25.
//
#include <iostream>
#include <chrono>
#include <thread>
#include <coroutine>

struct Task {
    struct promise_type {
        Task get_return_object() { return Task{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> coro;
    Task(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~Task() { coro.destroy(); }
};

struct Awaitable {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) const {
        std::thread([h]() {
            std::this_thread::sleep_for(std::chrono::seconds(2)); // 模拟IO
            h.resume();
        }).detach();
    }
    void await_resume() const noexcept {}
};

Task fetch_data() {
    std::cout << "Fetching data..." << std::endl;
    co_await Awaitable{};
    std::cout << "Data fetched!" << std::endl;
}

int main() {
    fetch_data();
    std::this_thread::sleep_for(std::chrono::seconds(3)); // 等待协程执行完
}

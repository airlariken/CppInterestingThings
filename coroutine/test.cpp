//
// Created by orange on 5/28/25.
//
#include <coroutine>
#include <future>
#include <iostream>
#include <string>
#include <thread>


using Callback = std::function<void(std::string)>;


// 自定义 Awaitable 封装 async 任务
struct AsyncTask {
    struct promise_type {
        std::promise<std::string> prom;
        AsyncTask get_return_object() {
            return AsyncTask{ prom.get_future() };
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(std::string value) {
            prom.set_value(value);
        }
        void unhandled_exception() {
            prom.set_exception(std::current_exception());
        }
    };

    std::future<std::string> future;
    explicit AsyncTask(std::future<std::string>&& f) : future(std::move(f)) {}
};

AsyncTask asyncTask() {
    co_return "hello from asyncTask()";
}
std::string run() {
    auto future = asyncTask().future;
    return future.get(); // 阻塞等待结果
}


int main() {

  std::cout << run() << std::endl;
  run();

}
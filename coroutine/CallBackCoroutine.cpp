#include <iostream>
#include <functional>
#include <coroutine>
#include <utility>

// 原来的 test1
void test1(int a) {
    std::cout << "test" << a << std::endl;
}

// 全局回调定义
using twoArgCallBack = std::function<void(int,int)>;
twoArgCallBack callBack;

// 注册、触发接口保持原样
void registerCallBack(twoArgCallBack cb) {
    callBack = std::move(cb);
}
void runCallBack(int t1, int t2) {
    if (callBack) callBack(t1, t2);
}

// 1. 定义一个最简单的 Task 协程类型
struct Task {
    struct promise_type {
        // 返回给调用者的 Task 对象
        Task get_return_object() noexcept {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        // 初始不挂起，直接进入协程体
        std::suspend_never initial_suspend() noexcept { return {}; }
        // 协程结束时也不挂起，直接销毁
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;
    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
};

// 2. 定义一个 Awaitable，用于 co_await 等待回调触发
struct CallbackAwaiter {
    std::coroutine_handle<Task::promise_type> h;  // 保存消费者协程
    int x, y;                                     // 用来存放回调参数

    bool await_ready() const noexcept { return false; }

    // suspend 时注册全局回调，并在被触发时 resume 协程
    void await_suspend(std::coroutine_handle<Task::promise_type> coro) noexcept {
        h = coro;
        registerCallBack([this](int a, int b){
            x = a;
            y = b;
            h.resume();
        });
    }

    // resume 后返回给 co_await 的结果
    std::pair<int,int> await_resume() const noexcept {
        return {x, y};
    }
};

// 3. 协程函数：在这里我们真正“等”到外部回调触发
Task callbackTask() {
    // 这一行会挂起协程，把控制权返回 main()
    auto [a, b] = co_await CallbackAwaiter{};

    // 恢复后继续执行
    std::cout << "Coroutine resumed with: " << a << ", " << b << std::endl;
    test1(a);
}

int main() {
    // 启动协程，运行到 co_await 处自动挂起
    callbackTask();

    std::cout << "Before triggering callback\n";

    // 仿原来 runCallBack，触发一次回调——协程会在此处 resume
    runCallBack(1, 2);

    return 0;
}

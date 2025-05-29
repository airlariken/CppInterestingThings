#include <iostream>
#include <coroutine>

struct Generator {
    struct promise_type {
        int current_value;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; } // 一开始挂起
        std::suspend_always final_suspend() noexcept { return {}; } // 结束时挂起（安全回收）
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {}; // 每次yield后挂起
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> coro;

    Generator(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~Generator() { if (coro) coro.destroy(); }

    bool next() {
        if (coro.done()) return false;
        coro.resume(); // 恢复协程执行
        return !coro.done();
    }

    int value() const {
        return coro.promise().current_value;
    }
};

Generator simple_counter() {
    for (int i = 1; i <= 5; ++i) {
        co_yield i; // 每次yield出去，挂起
    }
}

int main() {
    auto gen = simple_counter();
    while (gen.next()) {
        std::cout << "Yielded: " << gen.value() << std::endl;
    }
}

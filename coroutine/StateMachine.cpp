//
// Created by orange on 5/15/25.
//
#include <iostream>
#include <coroutine>
#include <string>

struct StateMachine {
    struct promise_type {
        std::string current_state;

        StateMachine get_return_object() { return StateMachine{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(std::string value) {
            current_state = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> coro;
    StateMachine(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~StateMachine() { if (coro) coro.destroy(); }

    std::string next() {
        coro.resume();
        return coro.done() ? "End" : coro.promise().current_state;
    }
};

StateMachine protocol_handler() {
    co_yield "Connecting";
    co_yield "Authenticating";
    co_yield "Transferring Data";
    co_yield "Closing Connection";
}

int main() {
    auto sm = protocol_handler();
    for (std::string state = sm.next(); state != "End"; state = sm.next()) {
        std::cout << "Current State: " << state << std::endl;
    }
}

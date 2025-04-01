//
// Created by orange on 3/11/25.
//

#include <functional>
#include <iostream>
#include <utility>

void test1(int a) {
    std::cout<<"test"<<a<<std::endl;
}
void nonCallFun() {
    return ;
}
using twoArgCallBack = std::function<void(int, int)>;
twoArgCallBack callBack;
void registerCallBack(twoArgCallBack cb) {
    callBack = std::move(cb);
}
void runCallBack(int t1, int t2) {
    callBack(t1,t2);
}
int main()
{
    int t = 0;
    // std::function<void(int)> func(std::bind(test1));
    // std::function<void()> func([t](int std::placeholders::_1,std::placeholders::_2) { test(t); });
    // func(3);
    registerCallBack(std::bind(test1, 1));
    registerCallBack([](int t1, int ) {return test1(t1);});
    // registerCallBack([](auto &&PL, auto &&PL2) {return test1(PL2);});
    runCallBack(-1,-2);
}
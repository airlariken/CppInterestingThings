//
// Created by orange on 5/15/25.
//
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>
class IntReader {
public:
        void BeginRead(const std::function<void(int*)>& cb) {
            std::thread _thread([cb]() {
                sleep(1);
                int value = 1;
                cb(&value);
            });
            _thread.detach();
        }
};
void printInt() {
    IntReader reader;
    reader.BeginRead([](int* value) {std::cout<<*value<<std::endl;});
}
int main(){
    printInt();
    while(1){};
    return 0;
}
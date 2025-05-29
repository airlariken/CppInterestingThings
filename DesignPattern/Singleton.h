//
// Created by orange on 4/2/25.
//
#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>
#include <iostream>
class Singleton {
private:
    int data = 0;
public:
    static Singleton* getInstance() {
        static Singleton instance;      // 局部静态变量，C++11 保证初始化时的线程安全
        return &instance;
    }
    void printData() const{
        std::cout<<"calling inner class nonstatic function, inner data is"<<data<<std::endl;
    }
    void modifyData(int _new_data) {data = _new_data;}
};
class SingletonWithMutex{        //用mutex实现多线程安全的单例
private:
    int data = 0;

    static std::mutex mutex_;
    static SingletonWithMutex* instance;
    SingletonWithMutex(){std::cout << "Singleton Constructor." << std::endl;};

    ~SingletonWithMutex()=default;
public:
    static SingletonWithMutex* getInstance() {
        {
            std::lock_guard<std::mutex> guard(mutex_);
            if (instance == nullptr)
                instance = new SingletonWithMutex();
        }
        return instance;        //这块有必要包含在锁内吗
    }
    void printData() const{
        std::cout<<"calling inner class nonstatic function, inner data is"<<data<<std::endl;
    }
    void modifyData(int _new_data) {data = _new_data;}
    //确保不会被调用
    SingletonWithMutex(const SingletonWithMutex&) = delete;
    SingletonWithMutex& operator=(const SingletonWithMutex&) = delete;

};
// 在类外定义静态成员和构造/析构函数
std::mutex SingletonWithMutex::mutex_;
SingletonWithMutex* SingletonWithMutex::instance = nullptr;


#include <atomic>

class SingletonWith2LockEfficient {
public:
    static SingletonWith2LockEfficient* getInstance() {
        SingletonWith2LockEfficient* tmp = m_instance.load(std::memory_order_acquire);
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            tmp = m_instance.load(std::memory_order_relaxed);
            if (tmp == nullptr) {
                tmp = new SingletonWith2LockEfficient();
                m_instance.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }
    void printData() const{
        std::cout<<"calling inner class nonstatic function, inner data is"<<data<<std::endl;
    }
    void modifyData(int _new_data) {data = _new_data;}
    // 删除拷贝构造函数和赋值运算符
    SingletonWith2LockEfficient(const SingletonWith2LockEfficient&) = delete;
    SingletonWith2LockEfficient& operator=(const SingletonWith2LockEfficient&) = delete;



private:
    SingletonWith2LockEfficient() = default;
    ~SingletonWith2LockEfficient() = default;
    std::atomic<int> data = 0;
    static std::atomic<SingletonWith2LockEfficient*> m_instance;
    static std::mutex m_mutex;
};

// 静态成员初始化
std::atomic<SingletonWith2LockEfficient*> SingletonWith2LockEfficient::m_instance{nullptr};
std::mutex SingletonWith2LockEfficient::m_mutex;

/* 使用示例：
Singleton::getInstance()->doSomething();
*/

#endif //SINGLETON_H
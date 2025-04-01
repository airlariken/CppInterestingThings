//
// Created by 橙子味 on 2025/4/1.
//

#ifndef SHAREDFROMTHIS_SHAREDPTR_WEAKPTR_H
#define SHAREDFROMTHIS_SHAREDPTR_WEAKPTR_H
#include <iostream>
#include <memory>
// Resource.h
#include <iostream>
#include <memory>

// 前向声明
class Owner;

class Resource {
public:
    // Resource() { std::cout << "Resource created\n"; }
    // ~Resource() { std::cout << "Resource destroyed\n"; }

    // 只做声明，不在类中直接实现
    void setOwner(std::shared_ptr<Owner> owner);
    void doSomething();

private:
    std::weak_ptr<Owner> m_owner;
};

#endif //SHAREDFROMTHIS_SHAREDPTR_WEAKPTR_H

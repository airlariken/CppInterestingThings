//
// Created by 橙子味 on 2025/4/1.
//
#include "Owner.h"
#include "Resource.h"
void Resource::setOwner(std::shared_ptr<Owner> owner) {
    m_owner = owner;
}
void Resource::doSomething() {
    if (std::shared_ptr<Owner> owner = m_owner.lock()) {  // 尝试获取 Owner 的 shared_ptr
        std::cout << "Resource is doing something with its Owner.\n";
        owner->notify();  // 调用 Owner 的方法
    } else {
        std::cout << "Owner is no longer available.\n";
    }
}
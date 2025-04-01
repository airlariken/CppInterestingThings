//
// Created by 橙子味 on 2025/4/1.
//
#include <iostream>
#include <memory>
#include "Owner.h"
#include "Resource.h"

int main() {
    // 创建一个 Owner 对象，使用 shared_ptr 管理
    std::shared_ptr<Owner> owner = std::make_shared<Owner>();

    // Owner 创建并绑定一个 Resource
    owner->createResource();

    // 调用 Resource 的方法，此时内部会尝试获取 Owner 的 shared_ptr
    std::shared_ptr<Resource> resource = owner->getResource();
    if (resource) {
        resource->doSomething();
    }

    // 当 owner 退出作用域时，Owner 和 Resource 都会被正确销毁
    return 0;
}

//
// Created by 橙子味 on 2025/4/1.
//

#ifndef OWNER_H
#define OWNER_H
#include <iostream>
#include <memory>
#include "Resource.h"
// Owner 类继承自 std::enable_shared_from_this，可以在成员函数中安全获得自身的 shared_ptr
class Owner : public std::enable_shared_from_this<Owner> {
public:
    Owner();
    ~Owner();

    // 创建 Resource 对象，并将自己传递给 Resource
    void createResource();

    // 一个简单的通知接口
    void notify();

    // 方便外部访问 Resource
    std::shared_ptr<Resource> getResource() const;

private:
    std::shared_ptr<Resource> m_resource;  // Owner 持有 Resource 的强引用
};
#endif //OWNER_H

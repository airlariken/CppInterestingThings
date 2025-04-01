//
// Created by orange on 4/1/25.
//
#include "Owner.h"
Owner::Owner() { std::cout << "Owner created\n"; }
Owner::~Owner() { std::cout << "Owner destroyed\n"; }

// 创建 Resource 对象，并将自己传递给 Resource
void Owner::createResource() {
    m_resource = std::make_shared<Resource>();
    // 通过 shared_from_this() 获取当前对象的 shared_ptr，传递给 Resource
    m_resource->setOwner(shared_from_this());
}

// 一个简单的通知接口
void Owner::notify(){
    std::cout << "Owner has been notified by Resource.\n";
}

// 方便外部访问 Resource
std::shared_ptr<Resource> Owner::getResource() const {
    return m_resource;
}
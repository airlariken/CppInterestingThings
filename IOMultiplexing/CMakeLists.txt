cmake_minimum_required(VERSION 3.15)
file(GLOB EXAMPLE_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# 给项目起个名字，比如 shared_ptr_demo
project(IOMultiplexing)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
# 将所有相关的 .cpp 文件放到一起生成一个可执行文件
# 为每个 .cpp 文件构建一个独立的可执行程序
add_executable(select_poll_epoll_test select_poll_epoll_test.cpp)

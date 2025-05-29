project(Corotine)

cmake_minimum_required(VERSION 3.15)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)

add_executable(IntReader IntReader.cpp)
add_executable(Generator Generator.cpp)
add_executable(AsyncAwait AsyncAwait.cpp)
add_executable(StateMachine StateMachine.cpp)
add_executable(CallBackCoroutine CallBackCoroutine.cpp)
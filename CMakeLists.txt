cmake_minimum_required(VERSION 3.15)
project(MultiExeDemo)

# 设置 C++ 标准（可根据项目需要修改）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 可执行文件1
#add_executable(exe_main main.cpp Resource.cpp Owner.cpp)
ADD_SUBDIRECTORY(muduoTechniques/sharedfromthis_sharedptr_weakptr)
ADD_SUBDIRECTORY(IOMultiplexing)
ADD_SUBDIRECTORY(ConnectionPool)
ADD_SUBDIRECTORY(muduoTechniques)
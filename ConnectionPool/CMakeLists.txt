cmake_minimum_required(VERSION 3.15)

# 给项目起个名字，比如 shared_ptr_demo
project(ConnectionPool)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
# 调用 mysql_config 获取编译选项
execute_process(
        COMMAND mysql_config --cflags
        OUTPUT_VARIABLE MYSQL_CFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 调用 mysql_config 获取链接选项
execute_process(
        COMMAND mysql_config --libs
        OUTPUT_VARIABLE MYSQL_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# 将 MYSQL_CFLAGS 中的选项添加到全局编译选项中
add_compile_options(${MYSQL_CFLAGS})

# 如果需要，可以打印调试信息
message(STATUS "MySQL CFLAGS: ${MYSQL_CFLAGS}")
message(STATUS "MySQL LIBS: ${MYSQL_LIBS}")

# 将所有相关的 .cpp 文件放到一起生成一个可执行文件
# 为每个 .cpp 文件构建一个独立的可执行程序
add_executable(ConnectionPool main.cpp)
# 可以直接使用 target_link_libraries 链接
target_link_libraries(ConnectionPool ${MYSQL_LIBS})
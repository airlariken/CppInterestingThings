//
// Created by orange on 3/10/25.
//
//
// Created by 橙子味 on 2025/3/5.
//
#include <iostream>
#include <vector>
#include <chrono>
#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstdlib>

#define FD_COUNT 500     // 测试的文件描述符数量
#define ITERATIONS 1000  // 迭代次数

int main() {
    // 用于存放所有管道的读端和写端
    std::vector<int> read_fds;
    std::vector<int> write_fds;

    // 创建 FD_COUNT 个管道，并在写端写入一个字节以保证读端总是处于“就绪”状态
    for (int i = 0; i < FD_COUNT; i++) {
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe error");
            exit(EXIT_FAILURE);
        }
        // 写入一个字节（不在意具体内容），这样读端将立即可读
        if (write(pipefd[1], "x", 1) != 1) {
            perror("write error");
            exit(EXIT_FAILURE);
        }
        read_fds.push_back(pipefd[0]);
        write_fds.push_back(pipefd[1]);
    }

    // 计算 select 需要的最大文件描述符数（select 的第一个参数要求为最大 fd+1）
    int max_fd = 0;
    for (int fd : read_fds) {
        if (fd > max_fd) max_fd = fd;
    }

    // --------------------- select 性能测试 ---------------------
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0; // 非阻塞调用
    auto start = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; iter++) {
        fd_set rfds;
        FD_ZERO(&rfds);
        // 将所有读端添加到 fd_set 中
        for (int fd : read_fds) {
            FD_SET(fd, &rfds);
        }
        int ret = select(max_fd + 1, &rfds, nullptr, nullptr, &tv);
        if (ret < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }
        // 此处不做进一步处理，仅测量 select 的开销
    }
    auto end = std::chrono::steady_clock::now();
    auto select_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // --------------------- poll 性能测试 ---------------------
    // 初始化 pollfd 数组，每个元素监控一个读端的可读事件
    std::vector<struct pollfd> pfds(FD_COUNT);
    for (int i = 0; i < FD_COUNT; i++) {
        pfds[i].fd = read_fds[i];
        pfds[i].events = POLLIN;
    }
    start = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; iter++) {
        // 每次调用前需要清除上一次的返回状态
        for (int i = 0; i < FD_COUNT; i++) {
            pfds[i].revents = 0;
        }
        int ret = poll(pfds.data(), FD_COUNT, 0); // 非阻塞调用：超时 0 毫秒
        if (ret < 0) {
            perror("poll error");
            exit(EXIT_FAILURE);
        }
    }
    end = std::chrono::steady_clock::now();
    auto poll_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // --------------------- epoll 性能测试 ---------------------
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1 error");
        exit(EXIT_FAILURE);
    }
    // 将所有读端加入 epoll 监控
    for (int fd : read_fds) {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
            perror("epoll_ctl error");
            exit(EXIT_FAILURE);
        }
    }
    std::vector<struct epoll_event> events(FD_COUNT);
    start = std::chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; iter++) {
        int ret = epoll_wait(epfd, events.data(), FD_COUNT, 0); // 非阻塞调用：超时 0 毫秒
        if (ret < 0) {
            perror("epoll_wait error");
            exit(EXIT_FAILURE);
        }
    }
    end = std::chrono::steady_clock::now();
    auto epoll_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // --------------------- 输出测试结果 ---------------------
    std::cout << "Performance over " << ITERATIONS << " iterations with " << FD_COUNT << " fds:" << std::endl;
    std::cout << "select: total " << select_duration << " us, average "
              << static_cast<double>(select_duration) / ITERATIONS << " us/iter" << std::endl;
    std::cout << "poll: total " << poll_duration << " us, average "
              << static_cast<double>(poll_duration) / ITERATIONS << " us/iter" << std::endl;
    std::cout << "epoll: total " << epoll_duration << " us, average "
              << static_cast<double>(epoll_duration) / ITERATIONS << " us/iter" << std::endl;

    // --------------------- 清理资源 ---------------------
    for (int fd : read_fds) {
        close(fd);
    }
    for (int fd : write_fds) {
        close(fd);
    }
    close(epfd);

    return 0;
}

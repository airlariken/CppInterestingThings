//
// Created by orange on 3/6/25.
//
#include <mysql/mysql.h>

#include <queue>
#include <mutex>
#include <condition_variable>

#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H
class MySQLConnectionPool {
public:
    // 构造函数中需要传入数据库连接信息以及初始连接数和最大连接数
    MySQLConnectionPool(const std::string& host,
                        const std::string& user,
                        const std::string& password,
                        const std::string& database,
                        int port,
                        int initialSize,
                        int maxSize)
        : host_(host), user_(user), password_(password), database_(database),
          port_(port), maxSize_(maxSize), currentCount_(0)
    {
        // 创建初始连接数，并放入连接池中
        for (int i = 0; i < initialSize; ++i) {
            MYSQL* conn = createConnection();
            if (conn) {
                pool_.push(conn);
                ++currentCount_;
            }
        }
    }

    // 析构函数关闭所有连接
    ~MySQLConnectionPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!pool_.empty()) {
            MYSQL* conn = pool_.front();
            pool_.pop();
            mysql_close(conn);
        }
    }

    // 获取一个数据库连接
    MYSQL* getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        // 如果连接池为空且总连接数未达到最大值，则新建一个连接
        if (pool_.empty() && currentCount_ < maxSize_) {
            MYSQL* conn = createConnection();
            if (conn) {
                ++currentCount_;
                return conn;
            } else {
                std::cerr << "新建连接失败！" << std::endl;
                return nullptr;
            }
        }
        // 如果连接池为空但已达到最大值，则等待其他线程释放连接
        while (pool_.empty()) {
            cond_.wait(lock);
        }
        // 从队列中取出一个连接
        MYSQL* conn = pool_.front();
        pool_.pop();
        return conn;
    }

    // 释放连接，将连接放回连接池
    void releaseConnection(MYSQL* conn) {
        if (!conn) return;
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.push(conn);
        // 通知等待获取连接的线程
        cond_.notify_one();
    }

private:
    // 根据数据库配置创建一个新的连接
    MYSQL* createConnection() {
        MYSQL* conn = mysql_init(nullptr);
        if (!conn) {
            std::cerr << "mysql_init 失败！" << std::endl;
            return nullptr;
        }
        // 连接到数据库，注意这里如果连接失败会输出错误信息
        if (!mysql_real_connect(conn, host_.c_str(), user_.c_str(),
                                password_.c_str(), database_.c_str(), port_,
                                nullptr, 0)) {
            std::cerr << "mysql_real_connect 失败: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            return nullptr;
        }
        return conn;
    }

    // 连接池数据
    std::queue<MYSQL*> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;

    // 数据库连接信息
    std::string host_;
    std::string user_;
    std::string password_;
    std::string database_;
    int port_;

    int maxSize_;       // 连接池最大连接数
    int currentCount_;  // 当前已创建的连接数
};


#endif //CONNECTIONPOOL_H

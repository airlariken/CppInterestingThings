//
// Created by orange on 3/6/25.
//
#include <iostream>
#include <string>
#include "ConnectionPool.h"
#include <random>
#include <thread>
// 生成指定长度的随机字符串，用于生成随机名字
std::string generateRandomString(size_t length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result;
    result.reserve(length);
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);
    for (size_t i = 0; i < length; ++i) {
        result.push_back(chars[dist(engine)]);
    }
    return result;
}

// 生成随机年龄，范围18-65
int generateRandomAge() {
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(18, 65);
    return dist(engine);
}

// 随机选择性别：'M' 或 'F'
std::string generateRandomSex() {
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, 1);
    return (dist(engine) == 0) ? "male" : "female";
}

// 每个线程插入随机数据的函数，插入字段为name, age, sex
void insertRandomData(MySQLConnectionPool& pool, int threadId, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        // 从连接池获取连接
        MYSQL* conn = pool.getConnection();
        if (!conn) {
            std::cerr << "线程 " << threadId << " 获取连接失败" << std::endl;
            continue;
        }
        // 生成随机数据
        std::string name = generateRandomString(8);  // 随机名字
        int age = generateRandomAge();
        std::string sex = generateRandomSex();
        std::string user_password = generateRandomString(8);  // 随机名字
        // 构造INSERT语句，注意id为自增，不用插入
        // 注意：password 字段值前后都要用单引号
        std::string query =
          "INSERT INTO `user` (name, age, sex, password) VALUES ('"
          + name + "', "
          + std::to_string(age) + ", '"
          + sex + "', '"
          + user_password + "')";
        if (mysql_query(conn, query.c_str()) != 0) {
            std::cerr << "线程 " << threadId << " 插入数据错误: " << mysql_error(conn) << std::endl;
        } else {
            // std::cout << "线程 " << threadId << " 插入数据: {name: " << name
            //           << ", age: " << age << ", sex: " << sex << "}" << std::endl;
        }
        // 释放连接
        pool.releaseConnection(conn);
        // 模拟间隔
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


int stressTest(std::string user, std::string password ) {
    // 数据库连接配置，根据实际情况修改
    std::string host = "192.168.3.1";
    std::string database = "chat";
    int port = 3306;

    // 初始化连接池：初始创建2个连接，最多允许10个连接
    MySQLConnectionPool pool(host, user, password, database, port, 2, 10);

    // 创建测试用的表，包含 id, name, age, sex 字段
    MYSQL* conn = pool.getConnection();
    if (conn) {
        const char* createTableQuery =
            "CREATE TABLE IF NOT EXISTS test_table ("
            "id INT AUTO_INCREMENT PRIMARY KEY, "
            "name VARCHAR(50) NOT NULL, "
            "age INT NOT NULL, "
            "sex CHAR(1) NOT NULL)";
        if (mysql_query(conn, createTableQuery) != 0) {
            std::cerr << "创建表错误: " << mysql_error(conn) << std::endl;
        }
        pool.releaseConnection(conn);
    } else {
        std::cerr << "创建表时获取连接失败" << std::endl;
        return 1;
    }

    // 计时器开始，记录测试开始时间
    auto startTime = std::chrono::high_resolution_clock::now();

    // 配置压力测试参数：线程数和每个线程的插入次数
    int numThreads = 1;          // 启动20个线程
    int iterationsPerThread = 10; // 每个线程插入200条数据
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(insertRandomData, std::ref(pool), i, iterationsPerThread);
    }
    for (auto& t : threads) {
        t.join();
    }

    // 计时器结束，计算总耗时
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "压力测试完成，总耗时: " << duration.count() << " 毫秒" <<"插入了"<<iterationsPerThread*numThreads<<"条数据"<< std::endl;
    std::cout <<"平均每条插入时间: " << (float)iterationsPerThread*numThreads/duration.count()<<"毫秒" << std::endl;
    return 0;
}
int runDemo() {
    // 数据库连接配置，根据实际情况修改
    std::string host = "192.168.3.1";
    std::string user = "root";
    std::string password = "zmxncbv.345";
    std::string database = "user";
    int port = 3306;

    // 初始化连接池：初始创建 2 个连接，最多允许 5 个连接
    MySQLConnectionPool pool(host, user, password, database, port, 2, 5);

    // 获取一个连接
    MYSQL* conn = pool.getConnection();
    if (conn) {
        // 执行简单的 SQL 查询
        const char* query = "SELECT VERSION()";
        if (mysql_query(conn, query) == 0) {
            MYSQL_RES* result = mysql_store_result(conn);
            if (result) {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row)
                    std::cout << "MySQL 版本: " << row[0] << std::endl;
                mysql_free_result(result);
            }
        } else {
            std::cerr << "查询失败: " << mysql_error(conn) << std::endl;
        }
        // 用完后释放连接
        pool.releaseConnection(conn);
    }

    return 0;
}
// 测试示例：从连接池获取连接，执行简单查询，然后释放连接
int main(int argc, char** argv) {
    if (argc < 3) return 1;
    stressTest(argv[1], argv[2]);
}

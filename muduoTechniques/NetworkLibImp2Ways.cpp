//
// Created by 橙子味 on 2025/3/25.
//
#include <functional>
#include <string>
#include <iostream>
#include <memory>
using std::function;
using std::string;
//network library:
class Connection {
public:
  string msg;   //表示发送出去的数据
  void send(const string &_msg) {
    msg = _msg;   //模拟将用户准备好的数据发送出去
  }
};
using connPtr = std::shared_ptr<Connection>;

class ServerUsingFunctional{
  typedef function<void(const connPtr&, string&)> _msg_callback_type;
  private:
  _msg_callback_type _cb;
  public:
  void regisMsgCallBacks(const _msg_callback_type &_f){this->_cb = _f;};
  void start(const connPtr& conn, string &buf){
  _cb(conn, buf);
  }
};
//network library end



//user define codes:
class MyServer  //不用继承，通过bind实现: public ServerUsingFunctional
{
  void onMessage(const connPtr& _conn, string& buf){ //编写回调函数
    std::cout<<"get message from buf, len is :"<<buf.length()<<std::endl;
    _conn->send(buf); //收到消息并且发送出去
  }
  std::unique_ptr<ServerUsingFunctional> _server;     // ServerUsingFunctional* _server;

  std::shared_ptr<Connection> _conn;
  void _init() {
    _server = std::make_unique<ServerUsingFunctional>();
    _conn = std::make_shared<Connection>();
  }
public:
  MyServer() {
    _init();
  }
  void run(){
    // _server->regisMsgCallBacks(bind(&MyServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));    //注册回调 基于bind
    _server->regisMsgCallBacks([this](auto&& arg1, auto&& arg2){
      onMessage(std::forward<decltype(arg1)>(arg1),std::forward<decltype(arg2)>(arg2));});    //另一种注册回调方式：lambda，但是要考虑perfect forwarding问题
    string buf = "http://127.0.0.1:8080";

    _server->start(_conn, buf);
    std::cout<<"response is :"<<_conn->msg<<std::endl;
  }

};


class ServerUsingVirtualFunc{
public:
  virtual ~ServerUsingVirtualFunc() = default;
  virtual void onMessage(const connPtr &conn, string &buf) = 0;
  virtual void run() = 0;
};


class MyServer2 : public ServerUsingVirtualFunc {
  std::unique_ptr<ServerUsingFunctional> _server;     // ServerUsingFunctional* _server;

  std::shared_ptr<Connection> _conn;
  void _init() {
    _server = std::make_unique<ServerUsingFunctional>();
    _conn = std::make_shared<Connection>();
  }
public:
  MyServer2() {
    _init();
  }
  void onMessage(const connPtr& _conn, string &buf) override {    //继承自父类虚函数，接口
    std::cout<<"get message from buf, len is :"<<buf.length()<<std::endl;
    _conn->send(buf); //收到消息并且发送出去

  }
void run() override{
    string buf = "http://127.0.0.1:8080";
    onMessage(_conn,buf);
    std::cout<<"response is :"<<_conn->msg<<std::endl;
  }
};


int main()
{
  // MyServer server;
  // server.run();
  ServerUsingVirtualFunc* server2_ptr = new MyServer2();
  server2_ptr->run();
}
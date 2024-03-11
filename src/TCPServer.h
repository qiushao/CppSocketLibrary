//
// Created by qiushao on 2/6/24.
//

#ifndef CPPSOCKETLIBRARY_TCPSERVER_H
#define CPPSOCKETLIBRARY_TCPSERVER_H

#include <cstdint>
#include <memory>
#include <thread>
#include <atomic>
#include <map>
#include "TCPConnection.h"

class EPoll;

class TCPServer {
public:
    explicit TCPServer(uint16_t port);
    virtual ~TCPServer();

    virtual bool start();
    virtual bool stop();

protected:
    virtual void onConnect(const std::shared_ptr<TCPConnection> &connection) {}
    virtual void onDisconnect(const std::shared_ptr<TCPConnection> &connection) {}
    // 需要返回读取到的数据长度，用于判断是否 disconnect 了
    virtual ssize_t onReadAvailable(const std::shared_ptr<TCPConnection> &connection) = 0;

private:
    bool initListenSocket();
    void onReadableEvent(int fd);
    void onAccept();

private:
    int listenPort_ = 0;
    int listenSocket_ = 0;
    EPoll *epoll_ = nullptr;
    std::map<int, std::shared_ptr<TCPConnection>> clients_;
};


#endif //CPPSOCKETLIBRARY_TCPSERVER_H

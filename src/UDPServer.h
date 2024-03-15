//
// Created by qiushao on 3/13/24.
//

#ifndef CPPSOCKETLIBRARY_UDPSERVER_H
#define CPPSOCKETLIBRARY_UDPSERVER_H

#include <cstdint>
#include <cstdio>
#include <memory>
#include <atomic>
#include <map>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <functional>
#include "UDPSocket.h"

class EPoll;

class UDPServer {
public:
    explicit UDPServer(uint16_t port);
    virtual ~UDPServer();

    void setOnReadCallback(std::function<ssize_t (const std::shared_ptr<UDPSocket> &udpSocket)> onReadCallback);
    virtual bool startUDPServer();
    virtual bool stopUDPServer();

    int getSocket();
    uint16_t getPort();

protected:
    virtual void onSocketError() {}

private:
    bool initSocket();
    void onReadableEvent(int fd);

private:
    EPoll *epoll_ = nullptr;
    uint16_t port_ = 0;
    std::shared_ptr<UDPSocket> udpSocket_ = nullptr;
    // 需要返回读取到的数据长度，用于判断是否有网络异常
    std::function<ssize_t (const std::shared_ptr<UDPSocket> &udpSocket)> onReadCallback_ = nullptr;
};


#endif //CPPSOCKETLIBRARY_UDPSERVER_H

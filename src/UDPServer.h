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
#include "UDPSocket.h"

class EPoll;

class UDPServer {
public:
    explicit UDPServer(uint16_t port);
    virtual ~UDPServer();

    virtual bool startUDPServer();
    virtual bool stopUDPServer();

    int getSocket();
    uint16_t getPort();

protected:
    virtual void onSocketError() {}
    // 需要返回读取到的数据长度，用于判断是否有网络异常
    virtual ssize_t onReadAvailable(const std::shared_ptr<UDPSocket> &udpSocket) = 0;

private:
    bool initSocket();
    void onReadableEvent(int fd);

private:
    EPoll *epoll_ = nullptr;
    uint16_t port_;
    std::shared_ptr<UDPSocket> udpSocket_;
};


#endif //CPPSOCKETLIBRARY_UDPSERVER_H

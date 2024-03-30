//
// Created by jingle on 2024/3/30.
//

#ifndef CPPSOCKETLIBRARY_TCPCLIENT_H
#define CPPSOCKETLIBRARY_TCPCLIENT_H


#include <string>
#include <functional>
#include <netinet/in.h>
#include "TCPSocket.h"

class EPoll;

class TCPClient {
public:
    TCPClient(std::string  peerIp, uint16_t port);
    virtual ~TCPClient();
    bool connect();
    void setOnReadCallback(std::function<ssize_t (TCPClient *tcpClient)> onReadCallback);

    ssize_t readData(uint8_t *data, size_t len);
    ssize_t readDataWaitAll(uint8_t *data, size_t len);
    ssize_t writeData(const void *data, size_t len);
    ssize_t writeDataWaitAll(const void *data, size_t len);

protected:
    virtual void onSocketError() {}

private:
    void onReadableEvent(int fd);

private:
    EPoll *epoll_ = nullptr;
    std::shared_ptr<TCPSocket> tcpSocket_ = nullptr;
    std::string peerIp_;
    uint16_t port_ = 0;
    // 需要返回读取到的数据长度，用于判断是否有网络异常
    std::function<ssize_t (TCPClient *tcpClient)> onReadCallback_ = nullptr;
};


#endif //CPPSOCKETLIBRARY_TCPCLIENT_H

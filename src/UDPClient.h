//
// Created by qiushao on 3/13/24.
//

#ifndef CPPSOCKETLIBRARY_UDPCLIENT_H
#define CPPSOCKETLIBRARY_UDPCLIENT_H

#include <string>
#include <memory>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "UDPSocket.h"

class EPoll;

class UDPClient {
public:
    UDPClient(const std::string& peerIp, uint16_t port);
    virtual ~UDPClient();
    void setOnReadCallback(std::function<ssize_t (UDPClient *udpClient)> onReadCallback);

    ssize_t readData(uint8_t *data, size_t len);
    ssize_t writeData(const void *data, size_t len);
    ssize_t writeDataWaitAll(const void *data, size_t len);

protected:
    virtual void onSocketError() {}

private:
    void onReadableEvent(int fd);

private:
    EPoll *epoll_ = nullptr;
    std::shared_ptr<UDPSocket> udpSocket_ = nullptr;
    std::string peerIp_;
    struct sockaddr_in peerSocketAddr_{};
    // 需要返回读取到的数据长度，用于判断是否有网络异常
    std::function<ssize_t (UDPClient *udpClient)> onReadCallback_ = nullptr;
};


#endif //CPPSOCKETLIBRARY_UDPCLIENT_H

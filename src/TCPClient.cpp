//
// Created by jingle on 2024/3/30.
//

#include "TCPClient.h"

#include <utility>
#include <cstring>
#include <arpa/inet.h>
#include "EPoll.h"
#include "log.h"

TCPClient::TCPClient(std::string  peerIp, uint16_t port) : peerIp_(std::move(peerIp)), port_(port) {

}

TCPClient::~TCPClient() {
    if (epoll_) {
        delete epoll_;
        epoll_ = nullptr;
    }
    tcpSocket_ = nullptr;
}

bool TCPClient::connect() {
    int socket_ = 0;
    if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOGE("create socket error:%s", strerror(errno));
        return false;
    }
    struct sockaddr_in peerSocketAddr_{};
    socklen_t socketLen_ = sizeof(struct sockaddr_in);
    bzero(&peerSocketAddr_, socketLen_);
    peerSocketAddr_.sin_family = AF_INET;
    peerSocketAddr_.sin_port = htons(port_);
    if (inet_pton(AF_INET, peerIp_.c_str(), &peerSocketAddr_.sin_addr) < 0) {
        LOGE("peer ip %s error:%s", peerIp_.c_str(), strerror(errno));
        return false;
    }

    if (::connect(socket_, (struct sockaddr *) &peerSocketAddr_, socketLen_) < 0) {
        LOGE("connect peer error:%s", strerror(errno));
        return false;
    }

    tcpSocket_ = std::make_shared<TCPSocket>(socket_, peerIp_, port_);
    tcpSocket_->setNonblock();
    epoll_ = new EPoll(8, std::bind(&TCPClient::onReadableEvent, this, std::placeholders::_1));
    epoll_->insertFd(tcpSocket_->getSocket());
    return true;
}

void TCPClient::onReadableEvent(int fd) {
    if (nullptr == onReadCallback_) {
        return;
    }
    auto ret = onReadCallback_(this);
    if (0 == ret) {
        onSocketError();
    }
}

ssize_t TCPClient::readData(uint8_t *data, size_t len) {
    return tcpSocket_->readData(data, len);
}

ssize_t TCPClient::readDataWaitAll(uint8_t *data, size_t len) {
    return tcpSocket_->readDataWaitAll(data, len);
}

ssize_t TCPClient::writeData(const void *data, size_t len) {
    return tcpSocket_->writeData(data, len);
}

ssize_t TCPClient::writeDataWaitAll(const void *data, size_t len) {
    return tcpSocket_->writeDataWaitAll(data, len);
}

void TCPClient::setOnReadCallback(std::function<ssize_t(TCPClient *)> onReadCallback) {
    onReadCallback_ = std::move(onReadCallback);
}


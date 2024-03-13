//
// Created by qiushao on 3/13/24.
//

#include <functional>
#include "UDPClient.h"
#include "EPoll.h"

UDPClient::UDPClient(const std::string& peerIp, uint16_t port)  {
    udpSocket_ = std::make_shared<UDPSocket>();
    udpSocket_->setNonblock();
    epoll_ = new EPoll(8, std::bind(&UDPClient::onReadableEvent, this, std::placeholders::_1));
    epoll_->insertFd(udpSocket_->getSocket());

    peerSocketAddr_.sin_family = AF_INET;
    peerSocketAddr_.sin_port = htons(port);
    peerSocketAddr_.sin_addr.s_addr = inet_addr(peerIp.c_str());
}

UDPClient::~UDPClient() {
    delete epoll_;
    epoll_ = nullptr;
    udpSocket_ = nullptr;
}

void UDPClient::onReadableEvent(int fd) {
    auto ret = onReadAvailable(udpSocket_);
    if (0 == ret) {
        onSocketError();
    }
}

ssize_t UDPClient::readData(uint8_t *data, size_t len) {
    struct sockaddr_in peerAddr{};
    return udpSocket_->readData(data, len, &peerAddr);
}

ssize_t UDPClient::writeData(const void *data, size_t len) {
    return udpSocket_->writeData(data, len, peerSocketAddr_);
}

ssize_t UDPClient::writeDataWaitAll(const void *data, size_t len) {
    return udpSocket_->writeDataWaitAll(data, len, peerSocketAddr_);
}

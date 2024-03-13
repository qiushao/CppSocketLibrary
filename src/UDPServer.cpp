//
// Created by qiushao on 3/13/24.
//

#include "UDPServer.h"
#include "EPoll.h"
#include "UDPSocket.h"
#include "log.h"

UDPServer::UDPServer(uint16_t port) : port_(port) {

}

UDPServer::~UDPServer() = default;

bool UDPServer::initSocket() {
    udpSocket_ = std::make_shared<UDPSocket>();
    return udpSocket_->bind(port_);
}

bool UDPServer::startUDPServer() {
    if (!initSocket()) {
        return false;
    }
    udpSocket_->setNonblock();
    epoll_ = new EPoll(8, std::bind(&UDPServer::onReadableEvent, this, std::placeholders::_1));
    epoll_->insertFd(udpSocket_->getSocket());
}

bool UDPServer::stopUDPServer() {
    LOGD("stop...");
    delete epoll_;
    epoll_ = nullptr;
    udpSocket_ = nullptr;
    return true;
}

void UDPServer::onReadableEvent(int fd) {
    auto ret = onReadAvailable(udpSocket_);
    if (0 == ret) {
        stopUDPServer();
        onSocketError();
    }
}

int UDPServer::getSocket() {
    return udpSocket_->getSocket();
}

uint16_t UDPServer::getPort() {
    return udpSocket_->getPort();
}

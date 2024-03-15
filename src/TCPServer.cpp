//
// Created by qiushao on 2/6/24.
//

#include <cstdio>
#include <cstring>
#include <csignal>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "TCPServer.h"
#include "EPoll.h"
#include "log.h"

TCPServer::TCPServer(uint16_t port) : listenPort_(port) {

}

TCPServer::~TCPServer() {
    LOGD("~TCPServer");
}

bool TCPServer::initListenSocket() {
    listenSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    int optValue = 1;
    setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof(optValue));

    struct sockaddr_in socketAddr{};
    bzero(&socketAddr, sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddr.sin_port = htons(listenPort_);
    if (bind(listenSocket_, (struct sockaddr *) &socketAddr, sizeof(struct sockaddr)) < 0) {
        LOGE("bind socket error:%s", strerror(errno));
        return false;
    }
    if (listen(listenSocket_, 8) < 0) {
        LOGE("listen socket error:%s", strerror(errno));
        return false;
    }
    return true;
}

bool TCPServer::start() {
    LOGD("start...");
    epoll_ = new EPoll(8, std::bind(&TCPServer::onReadableEvent, this, std::placeholders::_1));
    initListenSocket();
    epoll_->insertFd(listenSocket_);
    LOGD("listening %d ...", listenPort_);
    return true;
}

bool TCPServer::stop() {
    LOGD("stop...");
    delete epoll_;
    epoll_ = nullptr;
    shutdown(listenSocket_, SHUT_RDWR);
    close(listenSocket_);

    clients_.clear();
    LOGD("stop finish...");
    return true;
}

void TCPServer::onReadableEvent(int fd) {
    if (fd == listenSocket_) {
        onAccept();
    } else {
        auto it = clients_.find(fd);
        if (it != clients_.end()) {
            auto connection = it->second;
            auto len = onReadAvailable(connection);
            if (0 == len) {
                onDisconnect(connection);
                if (nullptr != epoll_) {
                    epoll_->removeFd(connection->getSocket());
                }
                clients_.erase(it);
                //LOGD("clients size = %zu", clients_.size());
            }

        } else {
            LOGE("unknown fd readable: %d", fd);
        }
    }
}

void TCPServer::onAccept() {
    struct sockaddr_in clientAddr{};
    socklen_t socketLen = sizeof(struct sockaddr_in);
    auto clientSocket = accept(listenSocket_, (struct sockaddr *) &clientAddr, &socketLen);
    if (clientSocket <= 0) {
        LOGE("accept socket(%d) error:%s", clientSocket, strerror(errno));
        return;
    }

    auto connection = std::make_shared<TCPSocket>(clientSocket, inet_ntoa(clientAddr.sin_addr), listenPort_);
    connection->setNonblock();
    clients_[clientSocket] = connection;
    if (nullptr != epoll_) {
        epoll_->insertFd(clientSocket);
    }
    onConnect(connection);
    //LOGD("clients size = %zu", clients_.size());
}

//
// Created by qiushao on 2/6/24.
//

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "TCPServer.h"
#include "EPoll.h"

TCPServer::TCPServer(uint16_t port) : listenPort_(port) {
    epoll_ = new EPoll(8, std::bind(&TCPServer::onReadableEvent, this, std::placeholders::_1));
}

TCPServer::~TCPServer() {
    printf("~TCPServer\n");
    delete epoll_;
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
        printf("bind socket error:%s\n", strerror(errno));
        return false;
    }
    if (listen(listenSocket_, 8) < 0) {
        printf("listen socket error:%s\n", strerror(errno));
        return false;
    }
    return true;
}

bool TCPServer::start() {
    printf("start...\n");
    initListenSocket();
    epoll_->insertFd(listenSocket_);
    printf("listening %d ...\n", listenPort_);
    return true;
}

bool TCPServer::stop() {
    printf("stop...\n");
    shutdown(listenSocket_, SHUT_RDWR);
    close(listenSocket_);

    clients_.clear();
    printf("stop finish...\n");
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
                epoll_->removeFd(connection->getSocket());
                clients_.erase(it);
                printf("clients size = %zu\n", clients_.size());
            }

        } else {
            printf("unknown fd readable: %d\n", fd);
        }
    }
}

void TCPServer::onAccept() {
    struct sockaddr_in clientAddr{};
    socklen_t socketLen = sizeof(struct sockaddr_in);
    auto clientSocket = accept(listenSocket_, (struct sockaddr *) &clientAddr, &socketLen);
    if (clientSocket <= 0) {
        printf("accept socket(%d) error:%s\n", clientSocket, strerror(errno));
        return;
    }

    auto connection = std::make_shared<TCPConnection>(clientSocket, inet_ntoa(clientAddr.sin_addr), listenPort_);
    clients_[clientSocket] = connection;
    epoll_->insertFd(clientSocket);
    onConnect(connection);
    printf("clients size = %zu\n", clients_.size());
}

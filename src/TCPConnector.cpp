//
// Created by qiushao on 3/8/24.
//

#include <cstring>
#include<arpa/inet.h>
#include<sys/socket.h>
#include "TCPConnector.h"
#include "log.h"

std::shared_ptr<TCPConnection> TCPConnector::connect(const std::string& serverIp, uint16_t port) {
    int socket_ = 0;
    std::shared_ptr<TCPConnection> connection_ = nullptr;
    if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOGE("create socket error:%s", strerror(errno));
        return connection_;
    }
    struct sockaddr_in peerSocketAddr_{};
    socklen_t socketLen_ = sizeof(struct sockaddr_in);
    bzero(&peerSocketAddr_, socketLen_);
    peerSocketAddr_.sin_family = AF_INET;
    peerSocketAddr_.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIp.c_str(), &peerSocketAddr_.sin_addr) < 0) {
        LOGE("peer ip %s error:%s", serverIp.c_str(), strerror(errno));
        return connection_;
    }

    if (::connect(socket_, (struct sockaddr *) &peerSocketAddr_, socketLen_) < 0) {
        LOGE("connect peer error:%s", strerror(errno));
        return connection_;
    }

    connection_ = std::make_shared<TCPConnection>(socket_, serverIp, port);
    return connection_;
}

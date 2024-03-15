//
// Created by qiushao on 3/13/24.
//

#include "UDPSocket.h"
#include <cerrno>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "log.h"

UDPSocket::UDPSocket() {
    initSocket();
}

UDPSocket::~UDPSocket() {
    close(socket_);
}

void UDPSocket::initSocket() {
    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int optValue = 1;
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof(optValue));
}

int UDPSocket::getSocket() const {
    return socket_;
}

uint16_t UDPSocket::getPort() const {
    return port_;
}

bool UDPSocket::setNonblock() const {
    int flags;
    flags = fcntl(socket_, F_GETFL, 0);
    if (flags == -1) {
        LOGE("fcntl get failed");
        return false;
    }

    flags |= O_NONBLOCK;
    if (fcntl(socket_, F_SETFL, flags) == -1) {
        LOGE("fcntl set failed");
        return false;
    }
    return true;
}

bool UDPSocket::bind(uint16_t port) {
    port_ = port;
    struct sockaddr_in mySockaddr{};
    memset(&mySockaddr, 0, sizeof(mySockaddr));
    mySockaddr.sin_family = AF_INET;
    mySockaddr.sin_port = htons(port_);
    mySockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //bind socket to port
    if (::bind(socket_, (struct sockaddr *) &mySockaddr, sizeof(mySockaddr)) < 0) {
        LOGE("bind socket error:%s", strerror(errno));
        return false;
    }

    socklen_t len = sizeof(mySockaddr);
    if (getsockname(socket_, (struct sockaddr *)&mySockaddr, &len) == -1) {
        LOGE("getsockname failed");
    } else {
        port_ = ntohs(mySockaddr.sin_port);
        LOGD("socket port = %d", port_);
    }

    return true;
}

ssize_t UDPSocket::readData(uint8_t *data, size_t len, struct sockaddr_in *peerInfo) const {
    socklen_t socketLen = sizeof(struct sockaddr_in);
    ssize_t ret = recvfrom(socket_, data, len, 0, (struct sockaddr *) peerInfo, &socketLen);

    if (ret < 0) {
        switch (errno) {
            case EAGAIN: // try again later
            case EINTR: // interrupted by signal, try again
                LOGD("should try again");
                return ret;
            case EFAULT:
            case EINVAL:
            case ENOMEM:
            case ENOTSOCK:
            case ECONNRESET:
                // handle the error and possibly terminate or log
                LOGE("recvfrom failed:%d:%s", errno, strerror(errno));
                return 0;
            default:
                // an unexpected error
                LOGE("recvfrom failed unknown error:%d:%s", errno, strerror(errno));
                return 0;
        }
    }
    return ret;
}

ssize_t UDPSocket::writeData(const void *data, size_t len, const struct sockaddr_in &peerInfo) const {
    return sendto(socket_, data, len, 0, (struct sockaddr *) &peerInfo, sizeof(struct sockaddr_in));
}

ssize_t UDPSocket::writeDataWaitAll(const void *data, size_t len, const struct sockaddr_in &peerInfo) const {
    while (true) {
        auto ret = sendto(socket_, data, len, 0, (struct sockaddr *) &peerInfo, sizeof(struct sockaddr_in));
        if (ret < 0) {
            switch (errno) {
                case EAGAIN: // try again later
                case EINTR: // interrupted by signal, try again
                    LOGD("try again");
                    continue;
                case EFAULT:
                case EINVAL:
                case ENOMEM:
                case ENOTSOCK:
                case ECONNRESET:
                    // handle the error and possibly terminate or log
                    LOGE("recvfrom failed:%d:%s", errno, strerror(errno));
                    return 0;
                default:
                    // an unexpected error
                    LOGE("recvfrom failed unknown error:%d:%s", errno, strerror(errno));
                    return 0;
            }
        }
        return ret;
    }
}

//
// Created by qiushao on 2/6/24.
//

#include "TCPConnection.h"
#include <unistd.h>
#include <sys/socket.h>
#include <thread>
#include <cstring>
#include <utility>

TCPConnection::TCPConnection(int socket, std::string peerIp, uint16_t port)
                            : socket_(socket), peerIp_(std::move(peerIp)), port_(port) {

}

TCPConnection::~TCPConnection() {
    printf("close connection\n");
    shutdown(socket_, SHUT_RDWR);
    close(socket_);
}

std::string TCPConnection::getPeerIP() {
    return peerIp_;
}

uint16_t TCPConnection::getPort() const {
    return port_;
}

ssize_t TCPConnection::readDataWaitAll(uint8_t *data, size_t len) const {
    ssize_t nRead;
    size_t nLeft = len;
    uint8_t *ptr = data;
    while (nLeft > 0) {
        nRead = recv(socket_, ptr, nLeft, MSG_WAITALL);
        if (nRead > 0) {
            nLeft -= nRead;
            ptr += nRead;
        } else if (nRead == 0) {
            printf("read = 0, peer disconnect\n");
            return nRead;
        } else {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                std::this_thread::sleep_for(std::chrono::milliseconds (5));
                continue;
            } else {
                printf("errno = %d:%s\n", errno, strerror(errno));
                return nRead;
            }
        }
    }
    return len - nLeft;
}

ssize_t TCPConnection::writeDataWaitAll(const void *data, size_t len) const {
    ssize_t nWritten;
    size_t nLeft = len;
    const char *ptr = static_cast<const char *>(data);
    while (nLeft > 0) {
        nWritten = send(socket_, ptr, nLeft, 0);
        if (nWritten > 0) {
            nLeft -= nWritten;
            ptr += nWritten;
        } else if (nWritten == 0) {
            printf("write = 0, disconnect\n");
            return nWritten;
        } else {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                std::this_thread::sleep_for(std::chrono::milliseconds (5));
                continue;
            } else{
                printf("errno = %d:%s\n", errno, strerror(errno));
                return nWritten;
            }
        }
    }
    return nWritten;
}

ssize_t TCPConnection::writeData(const void *data, size_t len) const {
    ssize_t nWritten;
    while (true) {
        nWritten = send(socket_, data, len, 0);
        if (nWritten > 0) {
            return nWritten;
        } else if (nWritten == 0) {
            printf("write = 0, disconnect\n");
            return nWritten;
        } else {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                std::this_thread::sleep_for(std::chrono::milliseconds (5));
                continue;
            } else{
                printf("errno = %d:%s\n", errno, strerror(errno));
                return nWritten;
            }
        }
    }
}

ssize_t TCPConnection::readData(uint8_t *data, size_t len) const {
    ssize_t nRead;
    while (true) {
        nRead = recv(socket_, data, len, 0);
        if (nRead > 0) {
            return nRead;
        } else if (nRead == 0) {
            printf("read = 0, peer disconnect\n");
            return nRead;
        } else {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                std::this_thread::sleep_for(std::chrono::milliseconds (5));
                continue;
            } else {
                printf("errno = %d:%s\n", errno, strerror(errno));
                return nRead;
            }
        }
    }
}

int TCPConnection::getSocket() const {
    return socket_;
}

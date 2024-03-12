//
// Created by qiushao on 2/6/24.
//

#include "TCPConnection.h"
#include <unistd.h>
#include <thread>
#include <cstring>
#include <utility>
#include <fcntl.h>
#include <sys/socket.h>
#include "log.h"

TCPConnection::TCPConnection(int socket, std::string peerIp, uint16_t port)
        : socket_(socket), peerIp_(std::move(peerIp)), port_(port) {
}

TCPConnection::~TCPConnection() {
    //LOGD("close connection");
    shutdown(socket_, SHUT_RDWR);
    close(socket_);
}

std::string TCPConnection::getPeerIP() {
    return peerIp_;
}

uint16_t TCPConnection::getPort() const {
    return port_;
}

int TCPConnection::getSocket() const {
    return socket_;
}

bool TCPConnection::setNonblock() const {
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

ssize_t TCPConnection::readDataWaitAll(uint8_t *data, size_t len) {
    if (broken_) {
        LOGE("connection is broken, can't read anymore");
        return 0;
    }
    ssize_t nRead;
    size_t nLeft = len;
    uint8_t *ptr = data;
    while (nLeft > 0) {
        nRead = recv(socket_, ptr, nLeft, MSG_WAITALL);
        if (nRead > 0) {
            nLeft -= nRead;
            ptr += nRead;
        } else if (nRead == 0) {
            // normal close
            LOGD("read = 0, peer disconnect");
            broken_ = true;
            return 0;
        } else {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                LOGD("errno = %d:%s, try again", errno, strerror(errno));
                // wait all 则需要继续等待接收数据
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            } else if (errno == EPIPE || errno == ECONNRESET) {
                LOGE("errno = %d:%s, connection is broken or close", errno, strerror(errno));
                broken_ = true;
                return 0;
            } else {
                LOGE("unknown errno = %d:%s", errno, strerror(errno));
                return nRead;
            }
        }
    }
    return len - nLeft;
}

ssize_t TCPConnection::writeDataWaitAll(const void *data, size_t len) {
    if (broken_) {
        LOGE("connection is broken, can't write anymore");
        return 0;
    }
    ssize_t nWritten;
    size_t nLeft = len;
    const char *ptr = static_cast<const char *>(data);
    while (nLeft > 0) {
        nWritten = send(socket_, ptr, nLeft, 0);
        if (nWritten > 0) {
            nLeft -= nWritten;
            ptr += nWritten;
        } else if (nWritten == 0) {
            LOGD("write = 0, peer disconnect");
            broken_ = true;
            return 0;
        } else {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
                LOGD("errno = %d:%s, try again", errno, strerror(errno));
                // wait all 则需要继续尝试发送
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            } else if (errno == EPIPE || errno == ECONNRESET) {
                LOGE("errno = %d:%s, connection is broken or close", errno, strerror(errno));
                broken_ = true;
                return 0;
            } else {
                LOGE("unknown errno = %d:%s", errno, strerror(errno));
                return nWritten;
            }
        }
    }
    return nWritten;
}

ssize_t TCPConnection::writeData(const void *data, size_t len) {
    if (broken_) {
        LOGE("connection is broken, can't write anymore");
        return 0;
    }
    ssize_t nWritten;
    nWritten = send(socket_, data, len, 0);
    if (nWritten > 0) {
        return nWritten;
    } else if (nWritten == 0) {
        LOGD("write = 0, peer disconnect");
        broken_ = true;
        return 0;
    } else {
        if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
            LOGD("errno = %d:%s, should try again", errno, strerror(errno));
            return nWritten;
        } else if (errno == EPIPE || errno == ECONNRESET) {
            LOGE("errno = %d:%s, connection is broken or close", errno, strerror(errno));
            broken_ = true;
            return 0;
        } else {
            LOGE("unknown errno = %d:%s", errno, strerror(errno));
            return nWritten;
        }
    }
}

ssize_t TCPConnection::readData(uint8_t *data, size_t len) {
    if (broken_) {
        LOGE("connection is broken, can't read anymore");
        return 0;
    }
    ssize_t nRead;
    nRead = recv(socket_, data, len, 0);
    if (nRead > 0) {
        return nRead;
    } else if (nRead == 0) {
        LOGD("read = 0, peer disconnect");
        broken_ = true;
        return 0;
    } else {
        if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) {
            LOGD("errno = %d:%s, should try again", errno, strerror(errno));
            return nRead;
        } else if (errno == EPIPE || errno == ECONNRESET) {
            LOGE("errno = %d:%s, connection is broken or close", errno, strerror(errno));
            broken_ = true;
            return 0;
        } else {
            LOGE("unknown errno = %d:%s", errno, strerror(errno));
            return nRead;
        }
    }
}

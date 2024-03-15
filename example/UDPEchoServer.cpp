//
// Created by qiushao on 3/13/24.
//
#include <cstring>
#include <unistd.h>
#include "log.h"
#include "UDPServer.h"
uint8_t buffer_[MAX_UDP_PAYLOAD];

ssize_t onReadAvailable(const std::shared_ptr<UDPSocket> &udpSocket) {
    memset(buffer_, 0, MAX_UDP_PAYLOAD);
    struct sockaddr_in peerAddr{};
    auto nRead = udpSocket->readData(buffer_, MAX_UDP_PAYLOAD, &peerAddr);
    if (nRead > 0) {
        LOGD("receive data:%s", buffer_);
        udpSocket->writeData(buffer_, nRead, peerAddr);
    } else if (nRead == 0) {
        LOGD("readData error, stop server");
    }
}

int main() {
    LOGD("udp echo server start");
    UDPServer server(10086);
    server.setOnReadCallback(onReadAvailable);
    server.startUDPServer();
    sleep(30);
    server.stopUDPServer();
    return 0;
}
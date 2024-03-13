//
// Created by qiushao on 3/13/24.
//
#include <cstring>
#include <unistd.h>
#include "log.h"
#include "UDPServer.h"

class UDPEchoServer : public UDPServer {
public:
    UDPEchoServer(uint16_t port) : UDPServer(port){

    }
    ~UDPEchoServer() override = default;

    ssize_t onReadAvailable(const std::shared_ptr<UDPSocket> &udpSocket) override {
        memset(buffer_, 0, MAX_UDP_PAYLOAD);
        struct sockaddr_in peerAddr{};
        auto nRead = udpSocket->readData(buffer_, MAX_UDP_PAYLOAD, &peerAddr);
        if (nRead > 0) {
            LOGD("receive data:%s", buffer_);
            udpSocket->writeData(buffer_, nRead, peerAddr);
        } else if (nRead == 0) {
            LOGD("readData error, stop server");
            stopUDPServer();
        }
    }

private:
    uint8_t buffer_[MAX_UDP_PAYLOAD];
};

int main() {
    LOGD("udp echo server start");
    UDPEchoServer server(10086);
    server.startUDPServer();
    sleep(30);
    server.stopUDPServer();
    return 0;
}
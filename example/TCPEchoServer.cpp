//
// Created by qiushao on 2/6/24.
//
#include <cstdio>
#include <csignal>
#include <cstring>
#include "TCPServer.h"
#include "log.h"

class TCPEchoServer : public TCPServer {
public:
    explicit TCPEchoServer(uint16_t port) : TCPServer(port) {

    }

    ~TCPEchoServer() override {
        LOGD("~TCPEchoServer");
    }

protected:
    void onConnect(const std::shared_ptr<TCPSocket> &connection) override {
        LOGD("onConnect peer(%s:%d)", connection->getPeerIP().c_str(), connection->getPort());
    }

    void onDisconnect(const std::shared_ptr<TCPSocket> &connection) override {
        LOGD("onDisconnect peer(%s:%d)", connection->getPeerIP().c_str(), connection->getPort());
    }

    ssize_t onReadAvailable(const std::shared_ptr<TCPSocket> &connection) override {
        char buffer[256];
        memset(buffer, 0, 256);
        auto len = connection->readData(reinterpret_cast<uint8_t *>(buffer), 256);
        if (len > 0) {
            connection->writeDataWaitAll(buffer, len);
        }
        return len;
    }
};

int main() {
    LOGD("tcp echo server start");
    TCPEchoServer server(10086);
    server.start();
    sleep(20);
    server.stop();
    return 0;
}
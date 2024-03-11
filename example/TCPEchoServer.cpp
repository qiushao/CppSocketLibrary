//
// Created by qiushao on 2/6/24.
//
#include <cstdio>
#include <csignal>
#include <cstring>
#include "TCPServer.h"

class TCPEchoServer : public TCPServer {
public:
    explicit TCPEchoServer(uint16_t port) : TCPServer(port) {

    }

    ~TCPEchoServer() override {
        printf("~TCPEchoServer\n");
    }

protected:
    void onConnect(const std::shared_ptr<TCPConnection> &connection) override {
        printf("onConnect peer(%s:%d)\n", connection->getPeerIP().c_str(), connection->getPort());
    }

    void onDisconnect(const std::shared_ptr<TCPConnection> &connection) override {
        printf("onDisconnect peer(%s:%d)\n", connection->getPeerIP().c_str(), connection->getPort());
    }

    ssize_t onReadAvailable(const std::shared_ptr<TCPConnection> &connection) override {
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
    printf("tcp echo server start\n");
    TCPEchoServer server(10086);
    server.start();
    sleep(5);
    server.stop();
    return 0;
}
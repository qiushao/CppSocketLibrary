//
// Created by qiushao on 2/7/24.
//
#include <cstdio>
#include <cstring>
#include <iostream>
#include "TCPConnector.h"
#include "EPoll.h"
#include "log.h"

static std::shared_ptr<TCPSocket> g_connection;
static EPoll *g_epoll;
static void onReceive(int fd) {
    char buffer[256];
    memset(buffer, 0, 256);
    auto nRead = g_connection->readData(reinterpret_cast<uint8_t *>(buffer), 256);
    if (nRead == 0) {
        LOGE("nRead == 0, disconnect");
        g_epoll->removeFd(g_connection->getSocket());
        return;
    }
    LOGD("server say: %s", buffer);
}

int main() {
    const char *serverIP = "127.0.0.1";
    uint16_t serverPort = 10086;
    g_connection = TCPConnector::connect(serverIP, serverPort);
    if (nullptr == g_connection) {
        LOGE("connect to server(%s:%d) failed", serverIP, serverPort);
        return -1;
    }

    LOGD("connect to server(%s:%d) success", serverIP, serverPort);
    g_connection->setNonblock();
    g_epoll = new EPoll(8, onReceive);
    g_epoll->insertFd(g_connection->getSocket());

    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "exit") {
            break;
        }
        auto nWrite = g_connection->writeDataWaitAll(line.c_str(), line.length());
        if (nWrite == 0) {
            LOGE("disconnect");
            break;
        }
    }

    LOGD("echo client exit");
    return 0;
}
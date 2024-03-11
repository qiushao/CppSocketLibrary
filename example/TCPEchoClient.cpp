//
// Created by qiushao on 2/7/24.
//
#include <cstdio>
#include <cstring>
#include <iostream>
#include "TCPConnector.h"
#include "log.h"

int main() {
    const char *serverIP = "127.0.0.1";
    uint16_t serverPort = 10086;
    auto connection = TCPConnector::connect(serverIP, serverPort);
    if (nullptr == connection) {
        LOGE("connect to server(%s:%d) failed", serverIP, serverPort);
        return -1;
    }

    LOGD("connect to server(%s:%d) success", serverIP, serverPort);
    char buffer[256];
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "exit") {
            break;
        }

        memset(buffer, 0, 256);
        connection->writeData(line.c_str(), line.length());
        connection->readData(reinterpret_cast<uint8_t *>(buffer), 256);
        LOGD("server say: %s", buffer);
    }

    return 0;
}
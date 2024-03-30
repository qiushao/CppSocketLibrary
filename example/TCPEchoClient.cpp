//
// Created by qiushao on 2/7/24.
//
#include <cstdio>
#include <cstring>
#include <iostream>
#include "EPoll.h"
#include "log.h"
#include "TCPClient.h"

constexpr uint16_t BUFFER_SIZE = 4096;
uint8_t buffer_[BUFFER_SIZE];
ssize_t onReadAvailable(TCPClient *client) {
    memset(buffer_, 0, BUFFER_SIZE);
    auto nRead = client->readData(buffer_, BUFFER_SIZE);
    if (nRead > 0) {
        LOGD("tcp server say:%s", buffer_);
    } else if (nRead == 0) {
        LOGD("readData error, exit client");
        exit(-1);
    }
}

int main() {
    const char *serverIP = "127.0.0.1";
    uint16_t serverPort = 10086;
    TCPClient client(serverIP, serverPort);
    if (!client.connect()) {
        LOGE("connect to server(%s:%d) failed", serverIP, serverPort);
        return -1;
    }

    LOGD("connect to server(%s:%d) success", serverIP, serverPort);
    client.setOnReadCallback(onReadAvailable);

    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "exit") {
            break;
        }
        auto nWrite = client.writeDataWaitAll(line.c_str(), line.length());
        if (nWrite == 0) {
            LOGE("disconnect");
            break;
        }
    }

    LOGD("echo client exit");
    return 0;
}
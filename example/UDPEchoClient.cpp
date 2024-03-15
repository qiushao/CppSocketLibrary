//
// Created by qiushao on 3/13/24.
//
#include <string>
#include <iostream>
#include <cstring>
#include <functional>
#include "log.h"
#include "UDPClient.h"

uint8_t buffer_[MAX_UDP_PAYLOAD];
ssize_t onReadAvailable(UDPClient *udpClient) {
    memset(buffer_, 0, MAX_UDP_PAYLOAD);
    auto nRead = udpClient->readData(buffer_, MAX_UDP_PAYLOAD);
    if (nRead > 0) {
        LOGD("udp server say:%s", buffer_);
    } else if (nRead == 0) {
        LOGD("readData error, exit client");
        exit(-1);
    }
}

int main() {
    LOGD("udp echo client start");
    UDPClient client("127.0.0.1", 10086);
    client.setOnReadCallback(onReadAvailable);
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "exit") {
            break;
        }
        auto nWrite = client.writeData(line.c_str(), line.length());
        if (nWrite == 0) {
            LOGE("send data failed");
            break;
        }
    }

    LOGD("udp echo client exit");
    return 0;
}

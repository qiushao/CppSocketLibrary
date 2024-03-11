//
// Created by qiushao on 2/7/24.
//
#include <cstdio>
#include <cstring>
#include <iostream>
#include "TCPConnector.h"

int main() {
    auto connection = TCPConnector::connect("127.0.0.1", 10086);
    if (nullptr == connection) {
        printf("connect server failed\n");
        return -1;
    }

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
        printf("receive from server: %s\n", buffer);
    }

    return 0;
}
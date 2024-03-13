//
// Created by qiushao on 3/13/24.
//
#include <string>
#include <iostream>
#include <cstring>
#include "log.h"
#include "UDPClient.h"

class UDPEchoClient : public UDPClient {
public:
    UDPEchoClient(const std::string& peerIp, uint16_t port) : UDPClient(peerIp, port) {

    }
    ~UDPEchoClient() override = default;

    ssize_t onReadAvailable(const std::shared_ptr<UDPSocket> &udpSocket) override {
        memset(buffer_, 0, MAX_UDP_PAYLOAD);
        auto nRead = readData(buffer_, MAX_UDP_PAYLOAD);
        if (nRead > 0) {
            LOGD("udp server say:%s", buffer_);
        } else if (nRead == 0) {
            LOGD("readData error, exit client");
            exit(-1);
        }
    }

private:
    uint8_t buffer_[MAX_UDP_PAYLOAD];
};

int main() {
    LOGD("udp echo client start");
    UDPEchoClient client("127.0.0.1", 10086);
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
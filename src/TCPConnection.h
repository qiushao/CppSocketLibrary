//
// Created by qiushao on 2/6/24.
//

#ifndef CPPSOCKETLIBRARY_TCPCONNECTION_H
#define CPPSOCKETLIBRARY_TCPCONNECTION_H

#include <string>
#include <functional>
#include <memory>

class TCPConnection {
public:
    TCPConnection(int socket, std::string peerIp, uint16_t port);
    virtual ~TCPConnection();

    std::string getPeerIP();
    uint16_t getPort() const;
    int getSocket() const;

    ssize_t readDataWaitAll(uint8_t *data, size_t len) const;
    ssize_t writeDataWaitAll(const void *data, size_t len) const;
    ssize_t readData(uint8_t *data, size_t len) const;
    ssize_t writeData(const void *data, size_t len) const;

private:
    int socket_;
    std::string peerIp_;
    uint16_t port_;
};


#endif //CPPSOCKETLIBRARY_TCPCONNECTION_H

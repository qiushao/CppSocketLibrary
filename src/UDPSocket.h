//
// Created by qiushao on 3/13/24.
//

#ifndef CPPSOCKETLIBRARY_UDPSOCKET_H
#define CPPSOCKETLIBRARY_UDPSOCKET_H

#include <cstdint>
#include <cstdio>
#include <string>

// MTU - IP header - UDP header
#define MAX_UDP_PAYLOAD 1472

class UDPSocket {
public:
    explicit UDPSocket();
    virtual ~UDPSocket();

    bool bind(uint16_t port);
    int getSocket() const;
    uint16_t getPort() const;
    bool setNonblock() const;

    ssize_t readData(uint8_t *data, size_t len, struct sockaddr_in *peerInfo) const;
    ssize_t writeData(const void *data, size_t len, const struct sockaddr_in &peerInfo) const;
    ssize_t writeDataWaitAll(const void *data, size_t len, const struct sockaddr_in &peerInfo) const;

private:
    void initSocket();
private:
    int socket_ = 0;
    uint16_t port_ = 0;
};


#endif //CPPSOCKETLIBRARY_UDPSOCKET_H

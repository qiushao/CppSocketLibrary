//
// Created by qiushao on 3/8/24.
//

#ifndef CPPSOCKETLIBRARY_TCPCONNECTOR_H
#define CPPSOCKETLIBRARY_TCPCONNECTOR_H

#include <string>
#include "TCPConnection.h"

class TCPConnector {
public:
    static std::shared_ptr<TCPConnection> connect(const std::string& serverIp, uint16_t port);
};


#endif //CPPSOCKETLIBRARY_TCPCONNECTOR_H

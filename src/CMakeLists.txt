add_library(CppSocket SHARED TCPServer.cpp TCPClient.cpp TCPSocket.cpp
        UDPServer.cpp UDPClient.cpp UDPSocket.cpp
        EPoll.cpp log.cpp init.cpp)
target_include_directories(CppSocket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
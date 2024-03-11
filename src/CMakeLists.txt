add_library(CppSocket SHARED TCPConnector.cpp TCPServer.cpp TCPConnection.cpp EPoll.cpp)
target_include_directories(CppSocket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
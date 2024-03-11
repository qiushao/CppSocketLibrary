add_library(CppSocket SHARED TCPConnector.cpp TCPServer.cpp TCPConnection.cpp EPoll.cpp log.cpp init.cpp)
target_include_directories(CppSocket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
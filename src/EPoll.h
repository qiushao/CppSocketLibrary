//
// Created by qiushao on 3/8/24.
//

#ifndef CPPSOCKETLIBRARY_EPOLL_H
#define CPPSOCKETLIBRARY_EPOLL_H

#include <cstdint>
#include <functional>
#include <thread>
#include <atomic>

class EPoll {
public:
    EPoll(int maxEvents, std::function<void(int fd)> readAvailableCallback);
    ~EPoll();

    bool insertFd(int fd) const;
    bool removeFd(int fd) const;

private:
    void eventLoop();
private:
    const int maxEvents_;
    int epollFd_ = -1;
    int exitEventFd_ = -1;
    std::function<void(int fd)> readAvailableCallback_;
    std::thread thread_;
    std::atomic<bool> interrupted_{};
};


#endif //CPPSOCKETLIBRARY_EPOLL_H

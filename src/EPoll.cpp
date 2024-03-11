#include <cstdio>
#include <cstring>
#include <utility>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "EPoll.h"

EPoll::EPoll(const int maxEvents, std::function<void(int)> readAvailableCallback) : maxEvents_(maxEvents) {
    readAvailableCallback_ = std::move(readAvailableCallback);
    epollFd_ = epoll_create1(0);
    if (epollFd_ <= 0) {
        printf("epoll_create error: %s\n", strerror(errno));
        return;
    }

    exitEventFd_ = eventfd(0, 0);
    if (exitEventFd_ <= 0) {
        printf("create eventfd error: %s\n", strerror(errno));
        return;
    }
    insertFd(exitEventFd_);

    interrupted_.store(false);
    thread_ = std::thread(&EPoll::eventLoop, this);
}

EPoll::~EPoll() {
    printf("~EPoll start\n");
    interrupted_.store(true);
    auto ret = eventfd_write(exitEventFd_, 1);
    if (ret != 0) {
        printf("eventfd_write error : ret = %d, error: %s\n", ret, strerror(errno));
    }
    thread_.join();
    close(epollFd_);
    printf("~EPoll end\n");
}

bool EPoll::insertFd(int fd) const {
    struct epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = fd;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
        printf("epoll_ctl add error: %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool EPoll::removeFd(int fd) const {
    struct epoll_event event{};
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) {
        printf("epoll_ctl del error: %s\n", strerror(errno));
        return false;
    }
    return true;
}

void EPoll::eventLoop() {
    while (!interrupted_.load()) {
        struct epoll_event events[maxEvents_];
        int count = epoll_wait(epollFd_, events, maxEvents_, -1);
        printf("epoll wait count = %d\n", count);
        if (count == -1) {
            printf("epoll_wait error: %s\n", strerror(errno));
            continue;
        }

        for (int i = 0; i < count; ++i) {
            if (events[i].data.fd == exitEventFd_) {
                printf("exit event\n");
                continue;
            }
            readAvailableCallback_(events[i].data.fd);
        }
    }
}

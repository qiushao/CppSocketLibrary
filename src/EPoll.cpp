#include <cstdio>
#include <cstring>
#include <utility>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "EPoll.h"
#include "log.h"

EPoll::EPoll(const int maxEvents, std::function<void(int)> readAvailableCallback) : maxEvents_(maxEvents) {
    readAvailableCallback_ = std::move(readAvailableCallback);
    epollFd_ = epoll_create1(0);
    if (epollFd_ <= 0) {
        LOGE("epoll_create error: %s", strerror(errno));
        return;
    }

    exitEventFd_ = eventfd(0, 0);
    if (exitEventFd_ <= 0) {
        LOGE("create eventfd error: %s", strerror(errno));
        return;
    }
    insertFd(exitEventFd_);

    interrupted_.store(false);
    thread_ = std::thread(&EPoll::eventLoop, this);
}

EPoll::~EPoll() {
    //LOGD("~EPoll start");
    interrupted_.store(true);
    auto ret = eventfd_write(exitEventFd_, 1);
    if (ret != 0) {
        LOGE("eventfd_write error : ret = %d, error: %s", ret, strerror(errno));
    }
    thread_.join();
    close(exitEventFd_);
    close(epollFd_);
    //LOGD("~EPoll end");
}

bool EPoll::insertFd(int fd) const {
    struct epoll_event event{};
    //event.events = EPOLLIN | EPOLLET; // 边缘触发，需要业务层循环读数据，直到返回 EAGAIN 或 EWOULDBLOCK。这表示当前没有更多数据可读
    event.events = EPOLLIN; // 默认水平触发，只要还有数据，就会继续有事件
    event.data.fd = fd;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
        LOGE("epoll_ctl add error: %s", strerror(errno));
        return false;
    }

    return true;
}

bool EPoll::removeFd(int fd) const {
    struct epoll_event event{};
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) {
        LOGE("epoll_ctl del error: %s", strerror(errno));
        return false;
    }
    return true;
}

void EPoll::eventLoop() {
    while (!interrupted_.load()) {
        struct epoll_event events[maxEvents_];
        int count = epoll_wait(epollFd_, events, maxEvents_, -1);
        // LOGD("epoll wait fd count = %d", count);
        if (count == -1) {
            LOGE("epoll_wait error: %s", strerror(errno));
            continue;
        }

        for (int i = 0; i < count; ++i) {
            if (events[i].data.fd == exitEventFd_) {
                LOGD("exit event loop");
                return;
            }
            readAvailableCallback_(events[i].data.fd);
        }
    }
}

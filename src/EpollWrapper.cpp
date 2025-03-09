#include "../include/EpollWrapper.hpp"

EpollWrapper::EpollWrapper() : epfd_(epoll_create(EPOLL_CLOEXEC)){
    assert(epfd_ > 0);
};

EpollWrapper::~EpollWrapper(){};

bool EpollWrapper::add_fd(int fd, uint32_t events){
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;

    if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event) < 0)
        return false;
    return true;
};

bool EpollWrapper::mod_fd(int fd, uint32_t events){
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;

    if(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event) < 0)
        return false;
    return true;
};

bool EpollWrapper::del_fd(int fd){
    if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr) < 0)
        return false;
    return true;
};
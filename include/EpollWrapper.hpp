#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

#include "../include/tinystl/vector.h"

#include <sys/epoll.h>
#include <assert.h>

class EpollWrapper{

    public:
    EpollWrapper();
    ~EpollWrapper();

    bool add_fd(int fd, uint32_t events);
    bool mod_fd(int fd, uint32_t events);
    bool del_fd(int fd);
    int wait();
    mystl::vector<epoll_event>::iterator get_events();

    private:
    int epfd_;
    mystl::vector<epoll_event> events_;
};

#endif
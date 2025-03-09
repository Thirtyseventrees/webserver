#ifndef EPOLLWRAPPER_HPP
#define EPOLLWRAPPER_HPP

#include <sys/epoll.h>
#include <assert.h>

class EpollWrapper{

    public:
    EpollWrapper();
    ~EpollWrapper();

    bool add_fd(int fd, uint32_t events);
    bool mod_fd(int fd, uint32_t events);
    bool del_fd(int fd);

    private:
    int epfd_;
};

#endif
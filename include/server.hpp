#pragma once

#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "../include/tinystl/vector.h"
#include "../include/EpollWrapper.hpp"
#include "../include/ThreadPool.hpp"
#include "../include/HttpData.hpp"
#include "../include/file_utils.hpp"
#include "../include/HttpServer_util.hpp"

class server{
    public:
    server(const char* ip, uint16_t port);
    int start();
    void end();

    private:
    struct sockaddr_in address;
    int listen_sock_;
    bool stop;
    EpollWrapper ew;
    ThreadPool thread_pool;
};

void set_nonblocking(int fd);
std::string read_http_request(int fd);
bool send_http_response(int fd, const std::string &response);
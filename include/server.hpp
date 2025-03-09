#pragma once

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

class server{
    public:
    server(const char* ip, uint16_t port);
    int start();

    private:
    struct sockaddr_in address;
    int sock;
};
#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <unordered_map>
#include <set>

#include "../include/tinystl/vector.h"
#include "../include/EpollWrapper.hpp"
#include "../include/ThreadPool.hpp"
#include "../include/HttpData.hpp"
#include "../include/file_utils.hpp"
#include "../include/HttpServer_util.hpp"
#include "WebSocket_util.hpp"

class server{
    public:
    server(const char* ip, uint16_t http_port, uint16_t qt_port);
    int start();
    void end();

    private:
    struct sockaddr_in http_address;
    int http_listen_sock_;
    struct sockaddr_in qt_address;
    int qt_listen_sock_;
    bool stop;
    EpollWrapper ew;
    ThreadPool thread_pool;
};

enum connProto{
    HTTP,
    WEBSOCKET,
    QT,
    OTHER
};

class connection{
    public:
    int fd;
    connProto conn_type;
    std::string username;
    connection(int f, connProto t) : fd(f), conn_type(t), username(){};
};

void set_nonblocking(int fd);
std::string read_http_request(int fd);
bool send_http_response(int fd, const std::string &response);

extern std::map<std::string, std::shared_ptr<connection>> user_to_connection;
extern std::unordered_map<int, std::string> fd_to_user;
extern std::unordered_map<int, std::shared_ptr<connection>> connections;

#endif
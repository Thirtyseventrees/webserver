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

#include "HttpData.hpp"
#include "file_utils.hpp"
#include "server.hpp"

std::string read_http_request(int fd);
bool send_http_response(int fd, const std::string &response);
void response(int fd, EpollWrapper &ew);

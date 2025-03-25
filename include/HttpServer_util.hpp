#ifndef HTTPSERVER_UTIL
#define HTTPSERVER_UTIL

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
#include <unordered_map>
#include <functional>

#include "HttpData.hpp"
#include "file_utils.hpp"
#include "server.hpp"
#include "myjson.hpp"
#include "WebSocket_util.hpp"

std::string read_http_request(int fd);
bool send_http_response(int fd, const std::string &response);
void http_response(void* ptr, EpollWrapper &ew);

void handle_root(const HttpRequest&, HttpResponse&, void*);
void handle_login(const HttpRequest&, HttpResponse&, void*);
void handle_dashboard(const HttpRequest&, HttpResponse&, void*);
void handle_upgrade(const HttpRequest&, HttpResponse&, void*);

std::string get_cookie_value(const std::string& cookie_header, const std::string& key);

extern std::unordered_map<std::string, std::function<void(const HttpRequest&, HttpResponse&, void*)>> http_router;

#endif
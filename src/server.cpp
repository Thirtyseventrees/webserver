#include "../include/server.hpp"

#include <iostream>
#include <string.h>
#include <string>

static bool stop = false;

void set_nonblocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("[ERROR] fcntl F_GETFL failed");
        return;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("[ERROR] fcntl F_SETFL failed");
    }
}

std::string read_http_request(int fd){
    std::string request;
    char buffer[4096];

    while (true) {
        int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            request.append(buffer, bytes);
        } else if (bytes == 0) {
            std::cout << "[INFO] Client closed connection: " << fd << std::endl;
            close(fd);
            return "";
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  // 所有数据读取完毕
            } else {
                perror("[ERROR] recv failed");
                close(fd);
                return "";
            }
        }
    }
    return request;
}

bool send_http_response(int fd, const std::string &response) {
    size_t total_sent = 0;
    size_t response_size = response.size();

    while (total_sent < response_size) {
        ssize_t bytes_sent = send(fd, response.c_str() + total_sent, response_size - total_sent, 0);
        if (bytes_sent < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue; // 继续发送
            } else {
                perror("[ERROR] send failed");
                close(fd);
                return false;
            }
        }
        total_sent += bytes_sent;
    }
    return true;
}

void response(int fd){

    std::string request_ = read_http_request(fd);

    if(request_.size() == 0)
        return;

    HttpRequest http_request_ = parse_HttpRequest(request_);
    
    // std::cout << request_ << std::endl;

    const char* html_content =
    "<html><body><h1>Welcome to My Web Server</h1></body></html>";

    size_t content_length = strlen(html_content);

    // 2. 构造 HTTP 响应
    std::string response =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(content_length) + "\r\n"
        "\r\n" + std::string(html_content);

    // 3. 发送 HTTP 响应
    send_http_response(fd, response);
    shutdown(fd, SHUT_WR);
}

server::server(const char* ip, uint16_t port){
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_aton(ip, &address.sin_addr);
    listen_sock_ = -1;
};

int server::start(){
    if(listen_sock_ > 0)
        return -1;
    listen_sock_ = socket(PF_INET, SOCK_STREAM, 0);
    int ret = bind(listen_sock_, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    listen(listen_sock_, 1024);

    EpollWrapper ew;
    ew.add_fd(listen_sock_, EPOLLIN | EPOLLERR);

    ThreadPool thread_pool(10);

    while(!stop){

        int num_of_events = ew.wait();
        auto events_ = ew.get_events();
        for(int i = 0; i < num_of_events; ++i){

            int fd = events_[i].data.fd;
            uint32_t ev = events_[i].events;

            // listen socket
            if(fd == listen_sock_){
                if(ev & EPOLLIN){
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int connfd = accept(listen_sock_, (struct sockaddr*)&client_addr, &client_len);
                    if(connfd >= 0){
                        set_nonblocking(connfd);
                        ew.add_fd(connfd, EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET);
                    }
                    continue;
                }
                else{
                    std::runtime_error("listen socket error");
                }
            }
            // othre sockets
            else{
                if(ev & (EPOLLHUP | EPOLLERR)){
                    std::cout << "[INFO] Connection closed by client: " << fd << std::endl;
                    ew.del_fd(fd);
                    close(fd);
                }
                else if(ev & EPOLLIN){
                    thread_pool.add_task(response, fd);
                }
                continue;
            }
        }
    }

    return 0;
}
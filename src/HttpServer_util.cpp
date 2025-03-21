#include "../include/HttpServer_util.hpp"

std::string read_http_request(int fd){
    std::string request;
    char buffer[4096];

    while (true) {
        int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            request.append(buffer, bytes);
        } else if (bytes == 0) {
            //std::cout << "[INFO] Client closed connection: " << fd << std::endl;
            //close(fd);
            return "";
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  // 所有数据读取完毕
            } else {
                perror("[ERROR] recv failed");
                //close(fd);
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
                //close(fd);
                return false;
            }
        }
        total_sent += bytes_sent;
    }
    return true;
}

void response(int fd, EpollWrapper &ew){

    std::string request_ = read_http_request(fd);

    //std::cout << request_ << std::endl;

    if(request_.size() == 0){
        //shutdown(fd, SHUT_WR);
        return;
    }

    HttpRequest http_request_ = parse_HttpRequest(request_);
    
    std::cout << http_request_.body << std::endl;

    // 构造 HTTP 响应
    HttpResponse http_response_ = get_response(http_request_);
        
    // Send HTTP response
    send_http_response(fd, http_response_.HttpResponse_to_string());

    ew.mod_fd(fd, EPOLLONESHOT | EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLHUP | EPOLLET);
    if(!http_request_.keep_alive_)
        shutdown(fd, SHUT_WR);
}
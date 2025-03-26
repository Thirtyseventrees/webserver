#include "../include/HttpServer_util.hpp"

std::unordered_map<std::string, std::function<void(const HttpRequest&, HttpResponse&, void*)>> http_router = {
    {"/", handle_root},
    {"/favicon.ico", handle_root},
    {"/login", handle_login},
    {"/dashboard", handle_dashboard},
    {"/upgrade", handle_upgrade}
};

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

void http_response(void* ptr, EpollWrapper &ew){

    std::string request_ = read_http_request(((connection*)ptr)->fd);

    //std::cout << request_ << std::endl;

    if(request_.size() == 0){
        //shutdown(fd, SHUT_WR);
        return;
    }

    HttpRequest http_request_ = parse_HttpRequest(request_);

    // 构造 HTTP 响应
    HttpResponse http_response_;

    http_router[http_request_.url_](http_request_, http_response_, ptr);
        
    // Send HTTP response
    send_http_response(((connection*)ptr)->fd, http_response_.HttpResponse_to_string());

    ew.mod_fd(ptr, ((connection*)ptr)->fd, EPOLLONESHOT | EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLHUP | EPOLLET);
    if(!http_request_.keep_alive_ && http_request_.headers_.find("Upgrade") == http_request_.headers_.end()){
        std::cout << "response shutdown" << std::endl;
        shutdown(((connection*)ptr)->fd, SHUT_WR);
    }
}

void handle_root(const HttpRequest& request, HttpResponse& response, void* ptr){
    response = make_ok_response(request);
}

void handle_login(const HttpRequest& request, HttpResponse& response, void* ptr){
    std::string user;
    response = make_login_response(request);
}

void handle_dashboard(const HttpRequest& request, HttpResponse& response, void* ptr){
    auto iter = request.headers_.find("Cookie");
    if(iter == request.headers_.end() || get_cookie_value(iter->second, "session_id") != "12345"){
        std::cout << "cookie!!!" << std::endl;
        response.set_statusCode(403);
        response.set_body("Forbidden: no cookie found");
    }
    else{
        response = make_ok_response(request);
    }
}

void handle_upgrade(const HttpRequest& request, HttpResponse& response, void* ptr){
    ((connection*)ptr)->conn_type = WEBSOCKET;
    response = make_upgrade_response(request);
    user_to_connection[request.query_params_.at("user")] = connections[((connection*)ptr)->fd];
    fd_to_user[((connection*)ptr)->fd] = request.query_params_.at("user");
}




std::string get_cookie_value(const std::string& cookie_header, const std::string& key) {
    size_t pos = cookie_header.find(key + "=");
    if (pos == std::string::npos) return "";
    pos += key.length() + 1;
    size_t end = cookie_header.find(";", pos);
    return cookie_header.substr(pos, end - pos);
}
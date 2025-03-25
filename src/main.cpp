#include<pthread.h>
#include<iostream>
#include<functional>
#include <filesystem>

#include "../include/server.hpp"
#include "../include/ThreadPool.hpp"
#include "../include/EpollWrapper.hpp"
#include "../include/HttpData.hpp"

void test(int a){
    std::cout << "hello" << a << std::endl;
    return;
}

int main(int argc, char* argv[]){
    const char* ip = "172.23.197.6";
    uint16_t http_port = 12345;
    uint16_t qt_port = 45678;

    server s(ip, http_port, qt_port);
    s.start();

    // std::string http_request =
    // "GET /index.html?name=Alice&age=25 HTTP/1.1\r\n"
    // "Host: localhost:8080\r\n"
    // "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/133.0.0.0 Safari/537.36\r\n"
    // "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
    // "Accept-Encoding: gzip, deflate\r\n"
    // "Accept-Language: en-GB,en;q=0.9,zh-CN;q=0.8,zh;q=0.7,en-US;q=0.6\r\n"
    // "Connection: keep-alive\r\n"
    // "\r\n"; // 空行表示请求头结束

    // HttpRequest test = parse_HttpRequest(http_request);
    // std::cout << test.method_ << " " << test.version_ << std::endl;
    // std::cout << test.url_ << std::endl;

    // for (auto& [key, value] : test.headers_) {
    //     std::cout << key << ": " << value << "\n";
    // }

    return 0;
}
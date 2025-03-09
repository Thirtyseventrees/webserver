#include "../include/server.hpp"

#include "../include/tinystl/vector.h"

#include <iostream>
#include <string.h>
#include <string>

static bool stop = false;

void handle_sigint(int sig){
    stop = true;
}

void handle_client(int connfd) {
    char buffer[1024];
    while (true) {
        ssize_t bytes_received = recv(connfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "Client disconnected.\n";
            break;
        }
        buffer[bytes_received] = '\0';

        if(!strcpy(buffer, "quit"))
            break;

        std::cout << "Received: " << buffer << std::endl;

        // Echo message back
        send(connfd, buffer, bytes_received, 0);
    }
    close(connfd);
}

server::server(const char* ip, uint16_t port){
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_aton(ip, &address.sin_addr);
    sock = -1;
};

int server::start(){
    if(sock > 0)
        return -1;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    listen(sock, 5);
    signal(SIGINT, handle_sigint);

    while(!stop){
        // accept the client
        struct sockaddr_in client;
        socklen_t client_addrlength = sizeof(client);
        int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
        printf("ip address: %s \nport: %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        //********* */
        const char* html_content =
        "<html><body><h1>Welcome to My Web Server</h1></body></html>";
        size_t content_length = strlen(html_content);

        // 2. 构造 HTTP 响应
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + std::to_string(content_length) + "\r\n"
            "\r\n" + std::string(html_content);

        // 3. 发送 HTTP 响应
        send(connfd, response.c_str(), response.size(), 0);
        /****************** */

        break;
    }

    return 0;
}
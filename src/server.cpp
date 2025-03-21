#include "../include/server.hpp"

std::function<void()> signal_handler_;

void bridge(int signo){
    if(signal_handler_){
        signal_handler_();
    }
}

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

server::server(const char* ip, uint16_t port) : stop(false), ew(), thread_pool(24){
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_aton(ip, &address.sin_addr);
    listen_sock_ = -1;
};

void server::end(){
    stop = true;
    close(listen_sock_);
}

int server::start(){
    if(listen_sock_ > 0)
        return -1;
    listen_sock_ = socket(PF_INET, SOCK_STREAM, 0);
    int ret = bind(listen_sock_, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    listen(listen_sock_, 1024);

    ew.add_fd(listen_sock_, EPOLLIN | EPOLLERR);

    // 处理退出
    int event_fd = eventfd(0, EFD_NONBLOCK);
    ew.add_fd(event_fd, EPOLLIN);
    signal_handler_ = [&event_fd](){
        uint64_t val = 1;
        write(event_fd, &val, sizeof(val));
    };
    signal(SIGINT, bridge);

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
                        ew.add_fd(connfd, EPOLLONESHOT | EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLHUP | EPOLLET);
                    }
                    continue;
                }
                else{
                    throw std::runtime_error("listen socket error");
                }
            }
            else if(fd == event_fd){
                std::cout << "\n[INFO] CTRL+C detected, shutting down server...\n";
                end();
            }
            // othre sockets
            else{
                if(ev & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                    std::cout << "[INFO] Connection closed by client: " << fd << std::endl;
                    ew.del_fd(fd);
                    close(fd);
                }
                else if(ev & EPOLLIN){
                    // struct sockaddr_in local_addr;
                    // socklen_t addr_len = sizeof(local_addr);
                    // if (::getsockname(fd, (struct sockaddr*)&local_addr, &addr_len) == 0) {
                    //     int local_port = ntohs(local_addr.sin_port);
                    //     std::cout << local_port << std::endl;
                    // }
                    thread_pool.add_task(response, fd, std::ref(ew));
                }
                continue;
            }
        }
    }

    return 0;
}
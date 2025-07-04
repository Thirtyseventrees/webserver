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

std::map<std::string, std::shared_ptr<connection>> user_to_connection;
std::unordered_map<int, std::string> fd_to_user;
std::unordered_map<int, std::shared_ptr<connection>> connections;


server::server(const char* ip, uint16_t http_port, uint16_t qt_port) : stop(false), ew(), thread_pool(24){
    http_address.sin_family = AF_INET;
    http_address.sin_port = htons(http_port);
    inet_aton(ip, &http_address.sin_addr);
    http_listen_sock_ = -1;

    qt_address.sin_family = AF_INET;
    qt_address.sin_port = htons(qt_port);
    inet_aton(ip, &qt_address.sin_addr);
    qt_listen_sock_ = -1;
};

void server::end(){
    stop = true;
    close(http_listen_sock_);
}

int server::start(){
    if(http_listen_sock_ > 0)
        return -1;
    http_listen_sock_ = socket(PF_INET, SOCK_STREAM, 0);
    int ret = bind(http_listen_sock_, (struct sockaddr*)&http_address, sizeof(http_address));
    assert(ret != -1);
    listen(http_listen_sock_, 1024);
    connection* hl = new connection(http_listen_sock_, OTHER);
    ew.add_fd((void*)hl, http_listen_sock_, EPOLLIN | EPOLLERR);

    if(qt_listen_sock_ > 0)
        return -1;
    qt_listen_sock_ = socket(PF_INET, SOCK_STREAM, 0);
    ret = bind(qt_listen_sock_, (struct sockaddr*)&qt_address, sizeof(qt_address));
    assert(ret != -1);
    listen(qt_listen_sock_, 1024);
    connection* ql = new connection(qt_listen_sock_, OTHER);
    ew.add_fd((void*)ql, qt_listen_sock_, EPOLLIN | EPOLLERR);

    // 处理退出
    int event_fd = eventfd(0, EFD_NONBLOCK);
    connection* event_conn = new connection(event_fd, OTHER);
    ew.add_fd((void*)event_conn, event_fd, EPOLLIN);
    signal_handler_ = [&event_fd](){
        uint64_t val = 1;
        write(event_fd, &val, sizeof(val));
    };
    signal(SIGINT, bridge);

    while(!stop){
        int num_of_events = ew.wait();
        auto events_ = ew.get_events();
        for(int i = 0; i < num_of_events; ++i){

            void* ptr = events_[i].data.ptr;
            uint32_t ev = events_[i].events;

            // listen socket
            if(((connection*)ptr)->fd == http_listen_sock_){
                if(ev & EPOLLIN){
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int connfd = accept(http_listen_sock_, (struct sockaddr*)&client_addr, &client_len);
                    if(connfd >= 0){
                        set_nonblocking(connfd);
                        //std::cout << "accept fd = " << connfd << std::endl;
                        auto conn = std::make_shared<connection>(connfd, HTTP);
                        connections[connfd] = conn;
                        //connection* http_conn = new connection(connfd, HTTP);
                        ew.add_fd(conn.get(), connfd, EPOLLONESHOT | EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLHUP | EPOLLET);
                    }
                    continue;
                }
                else{
                    throw std::runtime_error("listen socket error");
                }
            }
            else if(((connection*)ptr)->fd == event_fd){
                std::cout << "\n[INFO] CTRL+C detected, shutting down server...\n";
                end();
            }
            // othre sockets
            else{
                if(ev & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                    int fd = ((connection*)ptr)->fd;
                    if(((connection*)ptr)->conn_type == WEBSOCKET){
                        user_to_connection.erase(fd_to_user[((connection*)ptr)->fd]);
                        fd_to_user.erase(((connection*)ptr)->fd);
                    }
                    //std::cout << "[INFO] Connection closed by client: " << ((connection*)ptr)->fd << std::endl;
                    connections.erase(fd);
                    close(fd);
                    ew.del_fd(fd);
                    //delete (connection*)ptr;
                }
                else if(ev & EPOLLIN){
                    switch (((connection*)ptr)->conn_type)
                    {
                    case HTTP:
                        thread_pool.add_task(http_response, ptr, std::ref(ew));
                        break;
                    case WEBSOCKET:
                        thread_pool.add_task(websocket_response, ptr, std::ref(ew));
                        break;
                    default:
                        break;
                    }
                }
                continue;
            }
        }
    }

    delete hl;
    delete ql;
    return 0;
}
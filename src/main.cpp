#include<pthread.h>
#include<iostream>
#include<functional>

#include "../include/server.hpp"
#include "../include/ThreadPool.hpp"
#include "../include/EpollWrapper.hpp"

void test(int a){
    std::cout << "hello" << a << std::endl;
    return;
}

int main(int argc, char* argv[]){
    // const char* ip = "172.23.197.6";
    // uint16_t port = 12345;

    // server s(ip, port);
    // s.start();

    // ThreadPool pool(10);

    // for(int i = 0; i < 50; ++i){
    //     pool.add_task(test, i);
    // }

    // sleep(3);


    return 0;
}
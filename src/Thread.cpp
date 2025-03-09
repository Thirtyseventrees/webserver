#include "../include/Thread.hpp"

Thread::~Thread(){
    if(start_){
        pthread_detach(thread_id);
    }
}

void Thread::start(){
    if(!start_)
        start_ = (pthread_create(&thread_id, nullptr, thread_routine, this) == 0);
}

void Thread::join(){
    if(start_){
        pthread_join(thread_id, nullptr);
        start_ = false;
    }
}

void Thread::detach(){
    if(start_){
        pthread_detach(thread_id);
        start_ = false;
    }
}
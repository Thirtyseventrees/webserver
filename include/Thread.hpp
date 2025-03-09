#pragma once

#include <pthread.h>
#include <functional>
#include <utility>

class Thread{
    public:
    using thread_function = std::function<void()>;

    explicit Thread(thread_function func) : func_(func), thread_id(0), start_(false) {};

    template <typename Func, typename... Args>
    explicit Thread(Func&& func, Args&&... args) : func_(std::bind(std::forward<Func>(func), std::forward<Args>(args)...)), thread_id(0), start_(false) {};

    void start();
    void join();
    void detach();
    pthread_t get_thread_id(){ return thread_id; };

    ~Thread();

    private:
    thread_function func_;
    pthread_t thread_id;
    bool start_;
    static void* thread_routine(void* arg){
        Thread* thread = static_cast<Thread*>(arg);
        thread->func_();
        return nullptr;
    }
};
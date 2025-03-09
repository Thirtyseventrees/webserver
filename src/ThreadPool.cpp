#include "../include/ThreadPool.hpp"

ThreadPool::ThreadPool(int n) : nthreads(n), stop_(false), tg_(threads_){
    for(int i = 0; i < nthreads; ++i){
        threads_.push_back(std::thread(
            [this]{
                while(!stop_.load(std::memory_order_acquire)){
                    task_type task_;
                    {
                        std::unique_lock<std::mutex> ulk(this->mtx_);
                        this->cond_.wait(ulk, [this]{ return stop_.load(std::memory_order_acquire) || !this->tasks_.empty(); });
                        if(stop_.load(std::memory_order_acquire))
                            return;
                        task_ = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    task_();
                }
            }
        ));
    }
}

void ThreadPool::stop(){
    stop_.store(true, std::memory_order_release);
}

ThreadPool::~ThreadPool(){
    stop();
    cond_.notify_all();
}

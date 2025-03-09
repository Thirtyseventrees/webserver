#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include "./tinystl/vector.h"
#include "./tinystl/queue.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <type_traits>
#include <utility>

class ThreadsGuard{
    public:
    ThreadsGuard(mystl::vector<std::thread>& v)
        : threads_(v){}

    ~ThreadsGuard(){
        for (size_t i = 0; i != threads_.size(); ++i){
            if (threads_[i].joinable()){
                threads_[i].join();
            }
        }
    }

    private:
    ThreadsGuard(ThreadsGuard&& tg) = delete;
    ThreadsGuard& operator = (ThreadsGuard&& tg) = delete;

    ThreadsGuard(const ThreadsGuard&) = delete;
    ThreadsGuard& operator = (const ThreadsGuard&) = delete;

    private:
    mystl::vector<std::thread>& threads_;
};


class ThreadPool{
    public:
    using task_type = std::function<void()>;

    public:
    //Construct
    explicit ThreadPool(int n = 0);
    ~ThreadPool();

    void stop();

    template <typename func, typename... Args>
    std::future<typename std::result_of<func(Args...)>::type> add_task(func&& function, Args&&... args){
        using return_type = typename std::result_of<func(Args...)>::type;
        using task = std::packaged_task<return_type()>;
    
        auto task_ = std::make_shared<task>(std::bind(std::forward<func>(function), std::forward<Args>(args)...));
        auto ret = task_->get_future();
        {
            std::lock_guard<std::mutex> lg(mtx_);
            if(stop_.load(std::memory_order_acquire))
                throw std::runtime_error("thread pool has stopped");
            tasks_.emplace([task_]{ (*task_)(); });
        }
        cond_.notify_one();
        return ret;
    }

    private:
    int nthreads;
    std::atomic<bool> stop_;
    std::mutex mtx_;
    std::condition_variable cond_;
    mystl::queue<task_type> tasks_;
    mystl::vector<std::thread> threads_;
    ThreadsGuard tg_;
};

#endif
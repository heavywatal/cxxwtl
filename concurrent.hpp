#pragma once
#ifndef WTL_CONCURRENT_HPP_
#define WTL_CONCURRENT_HPP_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <vector>
#include <queue>
#include <memory>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// compatible with std::lock_guard<BasicLockable>
class Semaphore {
  public:
    explicit
    Semaphore(const unsigned int n=std::thread::hardware_concurrency()):
        count_(n) {}

    void lock() {
        std::unique_lock<std::mutex> lck(mutex_);
        condition_.wait(lck, [this]{return count_ > 0;});
        --count_;
    }

    void unlock() {
        std::lock_guard<std::mutex> lck(mutex_);
        ++count_;
        condition_.notify_one();
    }

    // Pythonic alias
    void acquire() {lock();}
    void release() {unlock();}

  private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned int count_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <typename T> inline
std::future_status status(const std::future<T>& future) {
    return future.wait_for(std::chrono::seconds(0));
}

template <typename T> inline
bool is_ready(const std::future<T>& future) {
    return status(future) == std::future_status::ready;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class BasicTask {
  public:
    virtual ~BasicTask() {}
    virtual void operator()() = 0;
};

template <typename result_t>
class Task: public BasicTask {
  public:
    Task(std::function<result_t()>&& func): std_task_(func) {}
    std::future<result_t> get_future() {return std_task_.get_future();}
    void operator()() override {std_task_();}
  private:
    std::packaged_task<result_t()> std_task_;
};

class ThreadPool {
  public:
    ThreadPool(const size_t n) {
        for (size_t i=0; i<n; ++i) {
            threads_.emplace_back(&ThreadPool::run, this);
        }
    }

    ~ThreadPool() {
        is_being_destroyed_ = true;
        condition_run_.notify_all();
        for (auto& th: threads_) {
            th.join();
        }
    }

    template <class Func>
    void submit(Func&& func) {
        std::lock_guard<std::mutex> lck(mutex_);
        tasks_.push(std::make_unique<Task<void>>(func));
        condition_run_.notify_one();
    }

    // TODO: std::invoke_result since C++17
    template <class Func, class... Args>
    std::future<std::result_of_t<Func(Args...)>>
    submit(Func&& func, Args&&... args) {
        using result_t = std::result_of_t<Func(Args...)>;
        std::lock_guard<std::mutex> lck(mutex_);
        auto task = std::make_unique<Task<result_t>>(std::bind(func, args...));
        std::future<result_t> ftr = task->get_future();
        tasks_.push(std::move(task));
        condition_run_.notify_one();
        return ftr;
    }

    // wait for worker threads to finish all tasks without executing join()
    void wait() {
        std::unique_lock<std::mutex> lck(mutex_);
        condition_wait_.wait(lck, [this]{
            return tasks_.empty() && waiting_threads_ == threads_.size();
        });
    }

  private:
    void run() {
        std::unique_ptr<BasicTask> task = nullptr;
        while (true) {
            {
                std::unique_lock<std::mutex> lck(mutex_);
                ++waiting_threads_;
                condition_wait_.notify_one();
                condition_run_.wait(lck, [this] {
                    return !tasks_.empty() || is_being_destroyed_;
                });
                --waiting_threads_;
                if (tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            (*task)();
        }
    }

    std::vector<std::thread> threads_;
    std::queue<std::unique_ptr<BasicTask>> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_run_;
    std::condition_variable condition_wait_;
    bool is_being_destroyed_ = false;
    size_t waiting_threads_ = 0;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_CONCURRENT_HPP_ */

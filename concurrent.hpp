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
#include <type_traits>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// compatible with std::lock_guard<BasicLockable>
class Semaphore {
  public:
    explicit
    Semaphore(unsigned int n=std::thread::hardware_concurrency()) noexcept:
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
    BasicTask() noexcept = default;
    BasicTask(BasicTask&&) noexcept = default;
    BasicTask(const BasicTask&) = delete;
    virtual ~BasicTask() = default;
    virtual void operator()() = 0;
};

template <typename result_t>
class Task: public BasicTask {
  public:
    template <class Func>
    Task(Func&& func) noexcept: std_task_(std::forward<Func>(func)) {}
    Task(Task&&) noexcept = default;
    Task(const Task&) = delete;
    std::future<result_t> get_future() {return std_task_.get_future();}
    void operator()() override {std_task_();}
  private:
    std::packaged_task<result_t()> std_task_;
};

static_assert(!std::is_default_constructible<Task<void>>{}, "");
static_assert(!std::is_copy_constructible<Task<void>>{}, "");
static_assert(std::is_nothrow_move_constructible<Task<void>>{}, "");

class ThreadPool {
  public:
    ThreadPool(unsigned int n) {
        for (unsigned int i=0; i<n; ++i) {
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
        tasks_.push(std::make_unique<Task<void>>(std::forward<Func>(func)));
        condition_run_.notify_one();
    }

    template <class Func, class... Args>
    auto submit(Func&& func, Args&&... args) {
#if __cplusplus >= 201703L
        using result_t = std::invoke_result_t<Func, Args...>;
#else
        using result_t = std::result_of_t<Func(Args...)>;
#endif
        std::lock_guard<std::mutex> lck(mutex_);
        auto task = std::make_unique<Task<result_t>>(
            [&func, args...]{return func(args...);}
        );
        std::future<result_t> ftr = task->get_future();
        tasks_.push(std::move(task));
        condition_run_.notify_one();
        return ftr;
    }

    // wait for worker threads to finish all tasks without executing join()
    void wait() {
        std::unique_lock<std::mutex> lck(mutex_);
        condition_wait_.wait(lck, [this]{
            return tasks_.empty() &&
                (waiting_threads_ == static_cast<unsigned int>(threads_.size()));
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
    unsigned int waiting_threads_ = 0;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_CONCURRENT_HPP_ */

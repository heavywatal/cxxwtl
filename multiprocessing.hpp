// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <algorithm>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline unsigned int num_threads() {
    unsigned int n = std::thread::hardware_concurrency();
    n = std::min(n, 12U); // do not want to use HT in ilp15
    return std::max(n, 1U);
}


class Semaphore {
  public:
    Semaphore() = default;
    explicit Semaphore(const size_t n): count_(n) {}

    void lock() {
        std::unique_lock<std::mutex> lck(mutex_);
        condition_.wait(lck, [this](){return count_ > 0;});
        --count_;
    }

    bool try_unlock() {
        std::lock_guard<std::mutex> lck(mutex_);
        if (count_ > 0) {
            --count_;
            return true;
        } else {
            return false;
        }
    }

    void unlock() {
        std::lock_guard<std::mutex> lck(mutex_);
        ++count_;
        condition_.notify_one();
    }

  private:
    std::mutex mutex_;
    std::condition_variable condition_;
    size_t count_ = num_threads();
};

class Pool {
  public:
    explicit Pool(const size_t n=num_threads())
        : semaphore_(n) {}

    ~Pool() {join();}

    template <class Func> inline
    auto async_future (Func&& func) -> std::future<decltype(func())> {
        semaphore_.lock();
        return std::async(std::launch::async, [&] {
            auto result = func();
            semaphore_.unlock();
            return result;
        });
    }

    template <class Func, class... Args> inline
    void async_thread (Func&& func, Args&&... args) {
        semaphore_.lock();
        // TODO: move capture
        workers_.emplace_back([&] {
            func(args...);
            semaphore_.unlock();
        });
    }

    void join() {
        for (auto& x: workers_) {x.join();}
        workers_.clear();
    }

  private:
    Semaphore semaphore_;
    std::vector<std::thread> workers_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

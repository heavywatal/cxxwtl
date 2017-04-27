// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_CONCURRENT_HPP_
#define WTL_CONCURRENT_HPP_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>

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
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_CONCURRENT_HPP_ */

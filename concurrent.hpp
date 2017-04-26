// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_CONCURRENT_HPP_
#define WTL_CONCURRENT_HPP_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

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
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_CONCURRENT_HPP_ */

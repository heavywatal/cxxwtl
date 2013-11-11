// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class Semaphore {
  public:
    explicit Semaphore(const size_t n=max_count_)
        : count_(n) {}

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
    size_t count_;
    static const size_t max_count_ = 4;
};

class SimplePool {
  public:
    explicit SimplePool(const size_t n)
        : semaphore_(n) {}

    template <class Func> inline
    auto async (Func&& func) -> std::future<decltype(func())> {
        semaphore_.lock();
        return std::async(std::launch::async, [&] {
            auto result = func();
            semaphore_.unlock();
            return result;
        });
    }

  private:
    Semaphore semaphore_;
};

template <class Return>
class Pool {
  public:
    explicit Pool(const size_t n)
        : semaphore_(n) {}

    template <class Func> inline
    void async (Func&& func) {
        semaphore_.lock();
        results_.push_back(std::async(std::launch::async, [&] {
            auto result = func();
            semaphore_.unlock();
            return result;
        }));
    }

    template <class Func> inline
    void async_void (Func&& func) {
        semaphore_.lock();
        results_.push_back(std::async(std::launch::async, [&] {
            func();
            semaphore_.unlock();
        }));
    }

    std::vector<std::future<Return> >&& get() {return std::move(results_);}

  private:
    Semaphore semaphore_;
    std::vector<std::future<Return> > results_;
};


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

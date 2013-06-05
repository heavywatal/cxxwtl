// -*- mode: c++; coding: utf-8 -*-
#ifndef MIXIN_HPP_
#define MIXIN_HPP_

#include <boost/utility.hpp>
#include <boost/thread.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// usage:
//  - public inheritance
//  - private constructor
//  - friend class Singleton<T>;

template <class T>
class SingletonWoBoost{
  public:
    static T* instance() {
        if (existance_==nullptr) {
            if (existance_==nullptr) {
                static T instance_; // survive until the end of the program!
                existance_ = &instance_;
            }
        }
        return existance_;
    }
    
  protected:
    SingletonWoBoost() {}
    ~SingletonWoBoost() {}
    
  private:
    SingletonWoBoost(const SingletonWoBoost&);
    T& operator=(const T&); // doesn't work well...
    
    static T* existance_;
};

template <class T>
T* SingletonWoBoost<T>::existance_ = nullptr;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class T>
class SynchronizedSingleton: boost::noncopyable {
  public:
    static T *instance() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        if (!instance_) {
            instance_ = new T;
        }
        return instance_;
    }
    
  private:
    static T* instance_;
    static boost::mutex mutex_;
};

template <class T>
T* SynchronizedSingleton<T>::instance_ = nullptr;

template <class T>
boost::mutex SynchronizedSingleton<T>::mutex_;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class T>
class OnceSingleton: boost::noncopyable{
  public:
    static T* instance() {
        static boost::once_flag flag(BOOST_ONCE_INIT);
        boost::call_once(init, flag);
        return instance_;
    }
    
  private:
    static void init() {
        instance_ = new T;
    }
    static T* instance_;
};

template <class T>
T* OnceSingleton<T>::instance_ = nullptr;


#endif /* MIXIN_HPP_ */

// -*- mode: c++; coding: utf-8 -*-
#ifndef COW_HPP_
#define COW_HPP_
 
#include <memory>

template <class T>
class cow_ptr{
    
  public:
    typedef std::shared_ptr<T> ref_ptr;
    
    cow_ptr(T* t): ptr_(t) {}
    cow_ptr(const ref_ptr& refptr): ptr_(refptr) {}
    cow_ptr(const cow_ptr& cowptr): ptr_(cowptr.ptr_) {}
    
    cow_ptr& operator=(const cow_ptr& rhs) {
        ptr_ = rhs.ptr_; // no need to check for self-assignment with shared_ptr
        return *this;
    }
    
    const T& operator*() const {return *ptr_;}
    T& operator*() {detach(); return *ptr_;}
    
    const T* operator->() const {return ptr_.operator->();}
    T* operator->() {detach(); return ptr_.operator->();}
    
    
  private:
    void detach() {
        T* tmp = ptr_.get();
        if (!(tmp==nullptr || ptr_.unique())) {
            ptr_ = ref_ptr(new T(*tmp));
        }
    }
    
    ref_ptr ptr_;
};

#endif /* COW_HPP_ */

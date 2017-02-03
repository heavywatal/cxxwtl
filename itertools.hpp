// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_ITERTOOLS_HPP_
#define WTL_ITERTOOLS_HPP_

#include <vector>
#include <type_traits>

#include <boost/coroutine2/coroutine.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include "math.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace itertools {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Generator {
  public:
    typedef boost::coroutines2::coroutine<value_type> coro_t;
    typedef boost::multiprecision::cpp_int size_type;
    typedef typename std::remove_const<decltype(value_type().size())>::type value_size_t;

    Generator() = default;
    virtual ~Generator() = default;

    size_type count() const {return cnt_;}
    virtual size_type max_count() const = 0;
    typename coro_t::pull_type operator()(const size_type start=0) {
        return typename coro_t::pull_type([this,start](typename coro_t::push_type& yield){source(yield, start);});
    }
  protected:
    size_type cnt_ = 0;
    virtual void source(typename coro_t::push_type& yield, const size_type start) = 0;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Product final: public Generator<value_type> {
  public:
    using typename Generator<value_type>::coro_t;
    using typename Generator<value_type>::size_type;
    using typename Generator<value_type>::value_size_t;
    Product() = delete;
    explicit Product(const std::vector<value_type>& axes):
        Generator<value_type>(),
        axes_(axes),
        value_(axes_.size()),
        pos_(axes_.size()) {}
    ~Product() = default;

    void reset() {
        pos_ = axes_.size();
        this->cnt_ = 0;
    }
    virtual size_type max_count() const override {
        size_type n = 1;
        for (const auto& c: axes_) {n *= c.size();}
        return n;
    }

  private:
    virtual void source(typename coro_t::push_type& yield, const size_type start) override {
        if (--pos_ > 0) {
            const value_size_t n = axes_[pos_].size();
            for (value_size_t i=0; i<n; ++i) {
                value_[pos_] = axes_[pos_][i];
                source(yield, start);
            }
            ++pos_;
        } else {
            const value_size_t n = axes_[pos_].size();
            for (value_size_t i=0; i<n; ++i) {
                value_[pos_] = axes_[pos_][i];
                if (this->cnt_++ >= start) {
                    yield(value_type(value_));
                }
            }
            ++pos_;
        }
    }
    const std::vector<value_type> axes_;
    value_type value_;
    value_size_t pos_;
};

template <class value_type>
Product<value_type> product(const std::vector<value_type>& axes) {
    return Product<value_type>(axes);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class UniAxis final: public Generator<value_type> {
  public:
    using typename Generator<value_type>::coro_t;
    using typename Generator<value_type>::size_type;
    using typename Generator<value_type>::value_size_t;
    UniAxis() = delete;
    UniAxis(const std::vector<value_type>& axes, const value_type& center):
      Generator<value_type>(),
      axes_(axes), center_(center) {}
    ~UniAxis() = default;

    virtual size_type max_count() const override {
        size_type n = 0;
        for (const auto& c: axes_) {n += c.size();}
        return n;
    }

  private:
    virtual void source(typename coro_t::push_type& yield, const size_type start) override {
        for (size_t i=0; i<axes_.size(); ++i) {
            auto value = center_;
            const auto& axis = axes_[i];
            const auto l = axis.size();
            for (value_size_t j=0; j<l; ++j) {
                value[i] = axis[j];
                if (this->cnt_++ >= start) {
                    yield(value_type(value));
                }
            }
        }
    }
    const std::vector<value_type> axes_;
    const value_type center_;
};

template <class value_type>
UniAxis<value_type> uniaxis(const std::vector<value_type>& axes, const value_type& center) {
    return UniAxis<value_type>(axes, center);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Prototype final: public Generator<value_type> {
  public:
    using typename Generator<value_type>::coro_t;
    using typename Generator<value_type>::size_type;
    using typename Generator<value_type>::value_size_t;
    Prototype() = delete;
    Prototype(const std::vector<value_type>& main_axes, const std::vector<value_type>& vicinity):
      Generator<value_type>(),
      main_axes_(main_axes), product_(vicinity) {}
    ~Prototype() = default;

    virtual size_type max_count() const override {
        size_type n = 0;
        for (const auto& v: main_axes_) {n += v.size();}
        return n * product_.max_count();
    }

  private:
    virtual void source(typename coro_t::push_type& yield, const size_type start) override {
        for (size_t i=0; i<main_axes_.size(); ++i) {
            const auto& axis = main_axes_[i];
            const auto l = axis.size();
            for (const auto& center: product_()) {
                auto value = center;
                for (value_size_t j=0; j<l; ++j) {
                    value[i] = axis[j];
                    if (this->cnt_++ >= start) {
                        yield(value_type(value));
                    }
                }
            }
            product_.reset();
        }
    }
    const std::vector<value_type> main_axes_;
    Product<value_type> product_;
};

template <class value_type>
Prototype<value_type> prototype(const std::vector<value_type>& axes, const std::vector<value_type>& vicinity) {
    return Prototype<value_type>(axes, vicinity);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Simplex  final: public Generator<value_type> {
  public:
    using typename Generator<value_type>::coro_t;
    using typename Generator<value_type>::size_type;
    Simplex() = delete;
    explicit Simplex(const std::vector<value_type>& axes, const double sum=1.0):
        product_(axes), sum_(sum) {}

    void reset() {product_.reset();}
    size_type raw_count() const {return product_.count();}
    virtual size_type max_count() const override {return product_.max_count();}

  private:
    virtual void source(typename coro_t::push_type& yield, const size_type start) override {
        for (const auto& v: product_()) {
            if (wtl::equal(sum_, v.sum())) {
                if (this->cnt_++ >= start) yield(v);
            }
        }
    }
    Product<value_type> product_;
    const double sum_;
};

template <class value_type>
Simplex<value_type> simplex(const std::vector<value_type>& axes, const double sum=1.0) {
    return Simplex<value_type>(axes, sum);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::itertools
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_ITERTOOLS_HPP_

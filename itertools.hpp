// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_ITERTOOLS_HPP_
#define WTL_ITERTOOLS_HPP_

#include <vector>
#include <type_traits>
#include <limits>
#include <cmath>

#include <boost/coroutine2/coroutine.hpp>
#include <boost/multiprecision/cpp_int.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace itertools {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Product {
  private:
    typedef boost::coroutines2::coroutine<value_type> coro_t;
    const std::vector<value_type> columns_;
    value_type value_;
    typedef typename std::remove_const<decltype(value_.size())>::type size_type;
    size_type col_;
    boost::multiprecision::cpp_int cnt_ = 0;
  public:
    Product() = delete;
    explicit Product(const std::vector<value_type>& columns):
        columns_(columns),
        value_(columns_.size()),
        col_(columns_.size()) {}

    typename coro_t::pull_type operator()(const size_type start=0) {
        return typename coro_t::pull_type([this,start](typename coro_t::push_type& yield){source(yield, start);});
    }

    boost::multiprecision::cpp_int count() const {return cnt_;}
    boost::multiprecision::cpp_int count_max() const {
        boost::multiprecision::cpp_int i = 1;
        for (const auto& c: columns_) {i *= c.size();}
        return i;
    }

  private:
    void source(typename coro_t::push_type& yield, const size_type start) {
        if (--col_ > 0) {
            const size_type n = columns_[col_].size();
            for (size_type i=0; i<n; ++i) {
                value_[col_] = columns_[col_][i];
                source(yield, start);
            }
            ++col_;
        } else {
            const size_type n = columns_[col_].size();
            for (size_type i=0; i<n; ++i) {
                value_[col_] = columns_[col_][i];
                if (cnt_++ >= start) {
                    yield(value_type(value_));
                }
            }
            ++col_;
        }
    }
};

template <class value_type>
Product<value_type> product(const std::vector<value_type>& columns) {
    return Product<value_type>(columns);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Simplex {
    typedef boost::coroutines2::coroutine<value_type> coro_t;
    Simplex() = delete;
  public:
    explicit Simplex(const std::vector<value_type>& columns, const double sum=1.0):
        product_(columns), sum_(sum) {}

    typename coro_t::pull_type operator()(void) {
        return typename coro_t::pull_type([this](typename coro_t::push_type& yield){source(yield);});
    }

    boost::multiprecision::cpp_int count() const {return cnt_;}
    boost::multiprecision::cpp_int count_all() const {return product_.count();}
    boost::multiprecision::cpp_int count_max() const {return product_.count_max();}

  private:
    bool equals(double x) const {
        return std::fabs(x -= sum_) < std::numeric_limits<double>::epsilon();
    }

    void source(typename coro_t::push_type& yield) {
        for (const auto& v: product_()) {
            if (equals(v.sum())) {
                ++cnt_;
                yield(v);
            }
        }
    }
    Product<value_type> product_;
    const double sum_;
    boost::multiprecision::cpp_int cnt_ = 0;
};

template <class value_type>
Simplex<value_type> simplex(const std::vector<value_type>& columns, const double sum=1.0) {
    return Simplex<value_type>(columns, sum);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::itertools
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_ITERTOOLS_HPP_

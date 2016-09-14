// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_ITERTOOLS_HPP_
#define WTL_ITERTOOLS_HPP_

#include <vector>
#include <type_traits>
#include <limits>
#include <boost/coroutine2/coroutine.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace itertools {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Product {
    typedef boost::coroutines2::coroutine<value_type> coro_t;
    Product() = delete;
  public:
    explicit Product(const std::vector<value_type>& columns):
        columns_(columns),
        value_(columns_.size()),
        col_(columns_.size()) {}

    typename coro_t::pull_type operator()(void) {
        return typename coro_t::pull_type([this](typename coro_t::push_type& yield){source(yield);});
    }

  private:
    void source(typename coro_t::push_type& yield) {
        if (--col_ > 0) {
            const size_type n = columns_[col_].size();
            for (size_type i=0; i<n; ++i) {
                value_[col_] = columns_[col_][i];
                source(yield);
            }
            ++col_;
        } else {
            const size_type n = columns_[col_].size();
            for (size_type i=0; i<n; ++i) {
                value_[col_] = columns_[col_][i];
                yield(value_type(value_));
            }
            ++col_;
        }
    }

    const std::vector<value_type> columns_;
    value_type value_;
    size_t col_;
    typedef typename std::remove_const<decltype(value_.size())>::type size_type;
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
    explicit Simplex(const std::vector<value_type>& columns): product_(columns) {}

    typename coro_t::pull_type operator()(void) {
        return typename coro_t::pull_type([this](typename coro_t::push_type& yield){source(yield);});
    }

  private:
    bool equals_one(double x) {
        return std::fabs(x -= 1.0) < std::numeric_limits<double>::epsilon();
    }

    void source(typename coro_t::push_type& yield) {
        for (const auto& v: product_()) {
            if (equals_one(v.sum())) {yield(v);}
        }
    }
    Product<value_type> product_;
};

template <class value_type>
Simplex<value_type> simplex(const std::vector<value_type>& columns) {
    return Simplex<value_type>(columns);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::itertools
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_ITERTOOLS_HPP_

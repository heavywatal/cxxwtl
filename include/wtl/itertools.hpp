#pragma once
#ifndef WTL_ITERTOOLS_HPP_
#define WTL_ITERTOOLS_HPP_

#include <cstdint>
#include <vector>
#include <type_traits>

#include <boost/coroutine2/coroutine.hpp>

#include "numeric.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace itertools {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Generator {
  public:
    using coro_t = boost::coroutines2::coroutine<value_type>;
    using size_type = uintmax_t;
    using value_size_t = typename std::remove_const_t<decltype(value_type().size())>;

    Generator() noexcept = default;
    virtual ~Generator() noexcept = default;
    Generator(const Generator&) noexcept = default;

    size_type count() const noexcept {return cnt_;}
    size_type max_count() const noexcept {return max_cnt_;}
    double percent() const noexcept {return 100.0 * cnt_ / max_cnt_;}
    typename coro_t::pull_type operator()(size_type skip=0) {
        return typename coro_t::pull_type([this,skip](typename coro_t::push_type& yield){source(yield, skip);});
    }
  protected:
    virtual void source(typename coro_t::push_type& yield, const size_type skip) = 0;
    size_type cnt_ = 0;
    size_type max_cnt_ = 0;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Product final: public Generator<value_type> {
  public:
    using typename Generator<value_type>::coro_t;
    using typename Generator<value_type>::size_type;
    using typename Generator<value_type>::value_size_t;
    Product() = delete;
    explicit Product(const std::vector<value_type>& axes)
        : Generator<value_type>(),
          axes_(axes),
          value_(axes_.size()),
          pos_(axes_.size()) {
        this->max_cnt_ = 1;
        for (const auto& c: axes_) {this->max_cnt_ *= c.size();}
    }
    ~Product() = default;
    Product(const Product&) = default;

    void reset() {
        pos_ = axes_.size();
        this->cnt_ = 0;
    }

  private:
    void source(typename coro_t::push_type& yield, size_type skip) override {
        if (--pos_ > 0) {
            const auto n = axes_[pos_].size();
            for (auto i = decltype(n){}; i < n; ++i) {
                value_[pos_] = axes_[pos_][i];
                source(yield, skip);
            }
            ++pos_;
        } else {
            const auto n = axes_[pos_].size();
            for (auto i = decltype(n){}; i < n; ++i) {
                value_[pos_] = axes_[pos_][i];
                if (++this->cnt_ > skip) {
                    yield(value_type(value_));
                }
            }
            ++pos_;
        }
    }
    const std::vector<value_type>& axes_;
    value_type value_;
    value_size_t pos_;
};

template <class value_type>
Product<value_type> product(const std::vector<value_type>& axes) {
    return Product<value_type>(axes);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class UniAxes final: public Generator<value_type> {
  public:
    using typename Generator<value_type>::coro_t;
    using typename Generator<value_type>::size_type;
    using typename Generator<value_type>::value_size_t;
    UniAxes() = delete;
    UniAxes(const std::vector<value_type>& axes, const value_type& center)
        : Generator<value_type>(),
          axes_(axes),
          center_(center) {
        this->max_cnt_ = 0;
        for (const auto& c: axes_) {this->max_cnt_ += c.size();}
    }
    ~UniAxes() = default;
    UniAxes(const UniAxes&) = default;

  private:
    void source(typename coro_t::push_type& yield, size_type skip) override {
        const auto n_dim = axes_.size();
        for (auto i = decltype(n_dim){}; i < n_dim; ++i) {
            auto value = center_;
            const auto& axis = axes_[i];
            const auto l = axis.size();
            for (auto j = decltype(l){}; j < l; ++j) {
                value[i] = axis[j];
                if (++this->cnt_ > skip) {
                    yield(value_type(value));
                }
            }
        }
    }
    const std::vector<value_type>& axes_;
    const value_type center_;
};

template <class value_type>
class UniAxis final: public Generator<value_type> {
  public:
    using typename Generator<value_type>::coro_t;
    using typename Generator<value_type>::size_type;
    using typename Generator<value_type>::value_size_t;
    UniAxis() = delete;
    UniAxis(const value_type& axis, const value_type& center, value_size_t idx)
        : Generator<value_type>(),
          axis_(axis),
          center_(center),
          idx_(idx) {
        this->max_cnt_ = axis_.size();
    }
    ~UniAxis() = default;
    UniAxis(const UniAxis&) = default;

  private:
    void source(typename coro_t::push_type& yield, size_type skip) override {
        auto value = center_;
        const auto n = axis_.size();
        for (auto j = decltype(n){}; j < n; ++j) {
            value[idx_] = axis_[j];
            if (++this->cnt_ > skip) {
                yield(value_type(value));
            }
        }
    }
    const value_type& axis_;
    const value_type center_;
    const value_size_t idx_;
};

template <class value_type>
UniAxes<value_type> uniaxis(const std::vector<value_type>& axes, const value_type& center) {
    return UniAxes<value_type>(axes, center);
}

template <class value_type>
UniAxis<value_type> uniaxis(const value_type& axis, const value_type& center, size_t idx) {
    return UniAxis<value_type>(axis, center, idx);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class value_type>
class Simplex  final: public Generator<value_type> {
  public:
    using typename Generator<value_type>::coro_t;
    using typename Generator<value_type>::size_type;
    Simplex() = delete;
    explicit Simplex(const std::vector<value_type>& axes, double sum=1.0)
        : product_(axes),
          sum_(sum) {
        this->max_cnt_ = product_.max_count();
    }

    void reset() {product_.reset();}
    size_type raw_count() const {return product_.count();}

  private:
    void source(typename coro_t::push_type& yield, const size_type skip) override {
        for (const auto& v: product_()) {
            if (wtl::approx(sum_, v.sum())) {
                if (++this->cnt_ > skip) yield(v);
            }
        }
    }
    Product<value_type> product_;
    const double sum_;
};

template <class value_type>
Simplex<value_type> simplex(const std::vector<value_type>& axes, double sum=1.0) {
    return Simplex<value_type>(axes, sum);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::itertools
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_ITERTOOLS_HPP_

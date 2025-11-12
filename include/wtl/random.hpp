#pragma once
#ifndef WTL_RANDOM_HPP_
#define WTL_RANDOM_HPP_

#include "signed.hpp"

#include <cstdint>
#include <initializer_list>
#include <limits>
#include <numeric>
#include <random>
#include <vector>
#include <unordered_set>
#include <ios>

namespace wtl {

namespace detail {

constexpr inline
uint64_t as_uint64(uint32_t high, uint32_t low) noexcept {
    return (static_cast<uint64_t>(high) << 32u) + low;
}

template <class UIntType> constexpr inline
double as_canonical(UIntType x) noexcept {
    constexpr auto max_uint = std::numeric_limits<UIntType>::max();
    constexpr double e = std::numeric_limits<double>::epsilon();
    constexpr double denom = static_cast<double>(max_uint) * (1.0 + e);
    return static_cast<double>(x) / denom;
}

} // namespace detail

class random_device_64 {
  public:
    using result_type = uint64_t;
    static constexpr result_type min() {return 0u;}
    static constexpr result_type max() {return std::numeric_limits<result_type>::max();}

    result_type operator()() {
        return detail::as_uint64(std_rd(), std_rd());
    }
    double entropy() const noexcept {return std_rd.entropy();}
  private:
    std::random_device std_rd;
};

template <class URBG> inline
double generate_canonical(URBG& gen) {
    if constexpr (URBG::max() == std::numeric_limits<uint64_t>::max()) {
        return detail::as_canonical(gen());
    } else if constexpr (URBG::max() == std::numeric_limits<uint32_t>::max()) {
        return detail::as_canonical(detail::as_uint64(gen(), gen()));
    } else {
        return std::generate_canonical<double, std::numeric_limits<double>::digits>(gen);
    }
}

template <class Iter, class URBG> inline
Iter choice(Iter begin_, Iter end_, URBG& engine) {
    using diff_t = decltype(std::distance(begin_, end_));
    std::uniform_int_distribution<diff_t> uniform(0, std::distance(begin_, end_) - 1);
    std::advance(begin_, uniform(engine));
    return begin_;
}

//! Floyd's algorithm
//! fast if k << n
template <class Container, class IntType, class URBG> inline
std::vector<typename Container::value_type>
sample_floyd(const Container& src, const IntType k, URBG& engine) {
    const auto n = static_cast<IntType>(src.size());
    if (n < k) throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": n < k");
    std::unordered_set<IntType> existing_indices;
    std::vector<typename Container::value_type> dst;
    reserve(dst, k);
    std::uniform_int_distribution<IntType> uniform(0, n - 1);
    for (IntType upper = n - k; upper < n; ++upper) {
        IntType idx = std::uniform_int_distribution<IntType>(0, upper)(engine);
        if (existing_indices.insert(idx).second) {
            dst.push_back(at(src, idx));
        } else {
            dst.push_back(at(src, upper));
        }
    }
    return dst;
}

//! Fisher-Yates algorithm; Durstenfeld; Knuth's P
//! consistently fast; note that whole src is copied first
template <class Container, class IntType, class URBG> inline
std::vector<typename Container::value_type>
sample_fisher(const Container& src, const IntType k, URBG& engine) {
    const auto n = static_cast<IntType>(src.size());
    if (n < k) throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": n < k");
    std::vector<typename Container::value_type> dst(std::begin(src), std::end(src));
    const auto last = n - 1;
    for (IntType i=0; i < k; ++i) {
        std::swap(at(dst, std::uniform_int_distribution<IntType>(i, last)(engine)), at(dst, i));
    }
    resize(dst, k);
    return dst;
}

//! Knuth's algorithm S
//! fast if k / n is large
//! The order is not random
template <class Container, class IntType, class URBG> inline
std::vector<typename Container::value_type>
sample_knuth(const Container& src, const IntType k, URBG& engine) {
    const auto n = static_cast<IntType>(src.size());
    if (n < k) throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": n < k");
    std::vector<typename Container::value_type> dst;
    reserve(dst, k);
    IntType i = 0;
    for (; i<k; ++i) {
        dst.push_back(at(src, i));
    }
    std::uniform_int_distribution<IntType> uniform(0, k - 1);
    while (i < n) {
        if (std::bernoulli_distribution((1.0 / ++i) * k)(engine)) {
            at(dst, uniform(engine)) = at(src, i - 1);
        }
    }
    return dst;
}

//! sample integers from [0, n) without replacement
template <class T, class URBG> inline
std::unordered_set<T> sample(T n, T k, URBG& engine) {
    if (n < k) throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": n < k");
    std::unordered_set<T> existing_indices;
    for (T upper = n - k; upper < n; ++upper) {
        T x = std::uniform_int_distribution<T>(0, upper)(engine);
        if (!existing_indices.insert(x).second) {
            existing_indices.insert(upper);
        }
    }
    return existing_indices;
}

template <class Container, class IntType, class URBG> inline
std::vector<typename Container::value_type>
sample(const Container& src, const IntType k, URBG& engine) {
    const auto n = static_cast<IntType>(src.size());
    if (100 * k < n) {return sample_floyd(src, k, engine);}
    else if (5 * k < n) {return sample_fisher(src, k, engine);}
    else {return sample_knuth(src, k, engine);}
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// a variant that accepts double k parameter

template <class IntType = int>
class negative_binomial_distribution {
  public:
    using result_type = IntType;

    class param_type {
      public:
        using distribution_type = negative_binomial_distribution;
        explicit param_type(double k = 1.0, double p = 0.5) noexcept:
          _k(k), _p(p) {}
        double k() const noexcept {return _k;}
        double p() const noexcept {return _p;}
        friend bool operator==(const param_type& lhs, const param_type& rhs) noexcept {
            return (lhs._k == rhs._k) && (lhs._p == rhs._p);
        }
        friend bool operator!=(const param_type& lhs, const param_type& rhs) noexcept {
            return !(lhs == rhs);
        }
      private:
        double _k;
        double _p;
    };

    negative_binomial_distribution() noexcept: negative_binomial_distribution(1.0) {}
    explicit negative_binomial_distribution(double k, double p = 0.5) noexcept:
      _param(k, p) {}
    explicit negative_binomial_distribution(const param_type& parameter) noexcept:
      _param(parameter) {}
    ~negative_binomial_distribution() noexcept = default;

    void reset() noexcept {}

    template <class URBG>
    result_type operator()(URBG& engine) const {
        return operator()(engine, _param);
    }
    template <class URBG>
    result_type operator()(URBG& engine, const param_type& parameter) const {
        auto k = parameter.k();
        auto p = parameter.p();
        double lambda = std::gamma_distribution<double>(k, (1.0 - p) / p)(engine);
        return std::poisson_distribution<result_type>(lambda)(engine);
    }

    double k() const noexcept {return _param.k();}
    double p() const noexcept {return _param.p();}

    param_type param() const noexcept {return _param;}
    void param(const param_type& parameter) noexcept {_param = parameter;}

    result_type constexpr min() const noexcept {return 0;}
    result_type constexpr max() const noexcept {return std::numeric_limits<result_type>::max();}

    friend bool operator==(const negative_binomial_distribution& lhs,
                           const negative_binomial_distribution& rhs) noexcept {
        return lhs._param == rhs._param;
    }
    friend bool operator!=(const negative_binomial_distribution& lhs,
                           const negative_binomial_distribution& rhs) noexcept {
        return !(lhs == rhs);
    }

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& ost,
               const negative_binomial_distribution& dist) {
        ost << dist.param().p() << " " << dist.param().k();
        return ost;
    }
    template <class CharT, class Traits>
    friend std::basic_istream<CharT, Traits>&
    operator>>(std::basic_istream<CharT, Traits>& ist,
               negative_binomial_distribution& dist) {
        ist.flags(std::ios_base::skipws);
        double k;
        double p;
        ist >> k >> p;
        if (!ist.fail()) {
            dist.param(param_type(k, p));
        }
        return ist;
    }

  private:
    param_type _param;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class IntType = int>
class multinomial_distribution {
  public:
    using result_type = IntType;

    class param_type {
      public:
        using distribution_type = multinomial_distribution;
        template <class InputIterator>
        explicit param_type(InputIterator begin, InputIterator end):
          _p(begin, end) {
            if (_p.empty()) {
                _p.assign(1, 1.0);
                return;
            }
            const auto sum_p = std::reduce(_p.begin(), _p.end());
            if (sum_p == 0.0) {
                throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": zero probability");
            }
            for (auto& x: _p) x /= sum_p;
        }
        explicit param_type(std::initializer_list<double> wl = {1.0}):
          param_type(wl.begin(), wl.end()) {}
        const std::vector<double>& probabilities() const noexcept {return _p;}
        friend bool operator==(const param_type& lhs, const param_type& rhs) noexcept {
            return lhs._p == rhs._p;
        }
        friend bool operator!=(const param_type& lhs, const param_type& rhs) noexcept {
            return !(lhs == rhs);
        }
      private:
        std::vector<double> _p;
    };

    multinomial_distribution(): multinomial_distribution({1.0}) {}
    template <class InputIterator>
    explicit multinomial_distribution(InputIterator begin, InputIterator end):
      _param(begin, end) {}
    explicit multinomial_distribution(std::initializer_list<double> wl):
      _param(wl) {}
    explicit multinomial_distribution(const param_type& parameter):
      _param(parameter) {}
    ~multinomial_distribution() noexcept = default;

    template <class URBG>
    std::vector<result_type> operator()(URBG& engine, result_type n) const {
        return operator()(engine, n, _param);
    }
    template <class URBG>
    std::vector<result_type>
    operator()(URBG& engine, result_type n, const param_type& parameter) const {
        const auto& probs = parameter.probabilities();
        std::vector<result_type> res(probs.size());
        const auto last = probs.size() - 1u;
        double denom = 1.0;
        for (auto i = decltype(last){}; n > 0 && i < last; ++i) {
          std::binomial_distribution<result_type> binomial(n, probs[i] / denom);
          n -= (res[i] = binomial(engine));
          denom -= probs[i];
        }
        res[last] = n;
        return res;
    }

    const std::vector<double>& probabilities() const noexcept {return _param.probabilities();}

    param_type param() const noexcept {return _param;}
    void param(const param_type& parameter) noexcept {_param = parameter;}

    result_type constexpr min() const noexcept {return 0;}
    result_type constexpr max() const noexcept {return std::numeric_limits<result_type>::max();}

    friend bool operator==(const multinomial_distribution& lhs,
                           const multinomial_distribution& rhs) noexcept {
        return lhs._param == rhs._param;
    }
    friend bool operator!=(const multinomial_distribution& lhs,
                           const multinomial_distribution& rhs) noexcept {
        return !(lhs == rhs);
    }

  private:
    param_type _param;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Global definition/declaration

inline std::mt19937& mt() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

inline std::mt19937_64& mt64() {
    static std::mt19937_64 generator(random_device_64{}());
    return generator;
}

} // namespace wtl

#endif /* WTL_RANDOM_HPP_ */

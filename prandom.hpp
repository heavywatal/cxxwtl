// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef PRANDOM_HPP_
#define PRANDOM_HPP_

#include <cstddef> // ptrdiff_t
#include <cassert>
#include <random>
#include <limits> // numeric_limits
#include <functional> // bind
#include <unordered_set>

#define HAVE_SSE2
#define SFMT_MEXP 19937
#include <SFMT.h>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class Iter, class RNG> inline
Iter choice(Iter begin_, Iter end_, RNG& rng) {
    std::uniform_int_distribution<ptrdiff_t> uniform(0, std::distance(begin_, end_) - 1);
    std::advance(begin_, uniform(rng));
    return begin_;
}

template <class Container, class RNG> inline
Container sample(const Container& src, const size_t k, RNG& rng) {
    const size_t n = src.size();
    if (100 * k < n) {return sample_set(src, k, rng);}
    else if (5 * k < n) {return sample_shuffle(src, k, rng);}
    else {return sample_knuth(src, k, rng);}
}

//! fast if k << n
template <class Container, class RNG> inline
Container sample_set(const Container& src, const size_t k, RNG& rng) {
    const size_t n = src.size();
    assert(k < n);
    std::unordered_set<size_t> existing_indices;
    Container dst;
    dst.reserve(k);
    std::uniform_int_distribution<size_t> uniform(0, n - 1);
    size_t idx = 0;
    for (size_t i=0; i<k; ++i) {
        do {
            idx = uniform(rng);
        } while (!std::get<1>(existing_indices.insert(idx)));
        dst.push_back(src[idx]);
    }
    return dst;
}

//! consistently fast; note that whole src is copied first
template <class Container, class RNG> inline
Container sample_shuffle(Container src, const size_t k, RNG& rng) {
    size_t n = src.size();
    assert(k < n);
    Container dst;
    dst.reserve(k);
    for (size_t i=0; i<k; ++i) {
        --n;
        size_t idx = std::uniform_int_distribution<size_t>(0, n)(rng);
        dst.push_back(std::move(src[idx]));
        src[idx] = std::move(src[n]);
    }
    return dst;
}

//! fast if k / n is large
template <class Container, class RNG> inline
Container sample_knuth(const Container& src, const size_t k, RNG& rng) {
    const size_t n = src.size();
    assert(k <= n);
    Container samples;
    samples.reserve(k);
    size_t i = 0;
    std::uniform_int_distribution<size_t> uniform(0, k - 1);
    for (const auto& item: src) {
        if (++i <= k) {
            samples.push_back(item);
        } else if (std::bernoulli_distribution((1.0 / i) * k)(rng)) {
            samples[uniform(rng)] = item;
        }
    }
    return samples;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Generators

class sfmt19937{
  public:
    typedef unsigned int result_type;
    typedef sfmt_t state_type;

    static constexpr result_type min() {return 0U;}
    static constexpr result_type max() {return 4294967295U;}

    // constructors
    explicit sfmt19937(const result_type s) {seed(s);}
    explicit sfmt19937(const state_type& state): state_(state) {}
    sfmt19937(const sfmt19937&) = delete;

    // [0, 2^32-1]
    result_type operator()() {
        return sfmt_genrand_uint32(&state_);
    }

    // [0.0, 1.0)
    double canonical() {
        return sfmt_genrand_real2(&state_);
    }

    void seed(const result_type s) {
        sfmt_init_gen_rand(&state_, s);
    }

    void discard(unsigned long long n) {
        for (; n != 0ULL; --n) {(*this)();}
    }

    template<class Fn>
    std::function<typename Fn::result_type ()> bind(Fn&& function) {
        return std::bind(function, *this);
    }

    const state_type& getstate() const {return state_;}
    void setstate(const state_type& state) {state_ = state;}

  private:
    state_type state_;
};


class XorShift {
  public:
    typedef unsigned int result_type;
    typedef unsigned int state_type[4];

    static constexpr result_type min() {return 0U;}
    static constexpr result_type max() {
        return std::numeric_limits<result_type>::max();
    }

    // constructors
    explicit XorShift(const result_type s) {seed(s);}
    explicit XorShift(const state_type& state) {setstate(state);}
    XorShift(const XorShift&) = delete;

    // [0, 2^32-1]
    result_type operator()() {
        unsigned int t(state_[0] ^ (state_[0] << 11));
        state_[0] = state_[1];
        state_[1] = state_[2];
        state_[2] = state_[3];
        return state_[3] = (state_[3] ^ (state_[3] >> 19)) ^ (t ^ (t >> 8));
    }

    // [0.0, 1.0)
    double canonical() {
        return std::uniform_real_distribution<double>()(*this);
    }

    void seed(result_type s) {
        for (unsigned int i=0; i<4U; ++i) {
            state_[i] = s = 1812433253U * (s ^ (s >> 30)) + i;
        }
    }

    void discard(unsigned long long n) {
        for (; n != 0ULL; --n) {(*this)();}
    }

    template<class Fn>
    std::function<typename Fn::result_type ()> bind(Fn&& function) {
        return std::bind(function, *this);
    }

    const state_type& getstate() const {return state_;}
    void setstate(const state_type& state) {
        for (unsigned int i=0; i<4U; ++i) {
            state_[i] = state[i];
        }
    }

  private:
    state_type state_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Global definition/declaration

namespace detail {
    template <class T> class Holder {
      public:
        T& operator()(void) {return instance_;}
      private:
        static T instance_;
    };
    template <class T> T Holder<T>::instance_(std::random_device{}());
} // namespace detail

inline std::mt19937& mt() {
    return wtl::detail::Holder<std::mt19937>{}();
}

inline sfmt19937& sfmt() {
    return wtl::detail::Holder<sfmt19937>{}();
}

inline XorShift& xorshift() {
    return wtl::detail::Holder<XorShift>{}();
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

//! Pythonic RNG object
template <class Generator>
class Prandom{
  public:
    // typedefs
    typedef unsigned int result_type;
    typedef result_type argument_type;

    static constexpr result_type min() {return Generator::min();}
    static constexpr result_type max() {return Generator::max();}

    // constructors
    explicit Prandom(const result_type s=std::random_device{}())
    : seed_(s), generator_(s) {seed(s);}

    Prandom(const Prandom&) = delete;

    ////////////////////////////////////////
    // methods

    void seed(const result_type s) {seed_ = s; generator_.seed(seed_);}
    void discard(unsigned long long n) {generator_.discard(n);}

    ////////////////////
    // integer

    // [0, 2^32-1]
    result_type operator()() {return generator_();}
    // [0, n-1]
    unsigned int randrange(unsigned int n) {
        return std::uniform_int_distribution<unsigned int>(0, --n)(generator_);
    }
    // [a, b-1]
    int randrange(const int a, int b) {
        return randint(a, --b);
    }
    // [a, b]
    int randint(const int a, const int b) {
        return std::uniform_int_distribution<int>(a, b)(generator_);
    }

    ////////////////////
    // uniform real

    // [0.0, 1.0)
    double random() {
        return std::uniform_real_distribution<double>()(generator_);
    }
    // (0.0, 1.0]
    double random_oc() {return 1.0 - random();}
    // [0.0, n)
    double uniform(double n) {
        return std::uniform_real_distribution<double>(0, n)(generator_);
    }
    // [a, b)
    double uniform(double a, double b) {
        return std::uniform_real_distribution<double>(a, b)(generator_);
    }

    ////////////////////
    // continuous

    // E = 1/lambda, V = 1/lambda^2
    double exponential(const double lambda=1.0) {
        return std::exponential_distribution<double>(lambda)(generator_);
    }

    // Scale-free: E = ?, V = ?
    double power(const double k=1.0, const double min=1.0) {
        return min * std::pow(random_oc(), -1.0 / k);
    }

    // E = mu, V = sigma^2
    double gauss(const double mu=0.0, const double sigma=1.0) {
        return std::normal_distribution<double>(mu, sigma)(generator_);
    }
    double normal(const double mu=0.0, const double sigma=1.0) {return gauss(mu, sigma);}

    ////////////////////
    // discrete

    // return true with probability p
    // E = p, V = p(1-p)
    bool bernoulli(const double p=0.5) {
        return std::bernoulli_distribution(p)(generator_);
    }

    // The number of true in n trials with probability p
    // E = np, V = np(1-p)
    unsigned int binomial(const unsigned int n, const double p=0.5) {
        return std::binomial_distribution<unsigned int>(n, p)(generator_);
    }

    // The expected number of occurrences in an unit of time/space
    // E = V = lambda
    unsigned int poisson(const double lambda) {
        return std::poisson_distribution<unsigned int>(lambda)(generator_);
    }

    // The number of trials needed to get first true
    // E = (1-p)/p, V = (1-p)/p^2
    unsigned int geometric(const double p) {
        return std::geometric_distribution<unsigned int>(p)(generator_);
    }

    ////////////////////
    // sequence
    template <class Iter>
    Iter choice(Iter begin_, Iter end_) {
        return wtl::choice(begin_, end_, generator_);
    }

    template <class Container>
    Container sample(Container src, const size_t k) {
        wtl::sample(&src, k, generator_);
        return src;
    }

  private:
    unsigned int seed_;
    Generator generator_;
};

inline Prandom<sfmt19937>& prandom() {
    return wtl::detail::Holder<Prandom<sfmt19937> >{}();
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* PRANDOM_HPP_ */

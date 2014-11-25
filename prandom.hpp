// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef PRANDOM_HPP_
#define PRANDOM_HPP_

#define _USE_MATH_DEFINES /* #define M_constant in cmath (for VC++ compatibility) */
#include <cmath> // sqrt, sin, cos
#include <cassert>
#include <random>
#include <limits> // numeric_limits
#include <algorithm> // shuffle
#include <set>
#include <functional>

#define HAVE_SSE2
#define SFMT_MEXP 19937
#include <SFMT.h>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

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
    : seed_(s), generator_(s), flag_(false), t_(0.0), u_(0.0) {seed(s);}
    
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
    unsigned int randrange(unsigned int n) {return static_cast<unsigned int>(uniform(n));}
    // [a, b-1]
    int randrange(const int a, const int b) {return static_cast<int>(uniform(a, b));}
    // [a, b]
    int randint(const int a, int b) {return randrange(a, ++b);}
    
    ////////////////////
    // uniform real
    
    // [0.0, 1.0)
    double random() {return generator_.canonical();}
    // (0.0, 1.0]
    double random_oc() {return 1.0 - random();}
    // [0.0, n)
    double uniform(double n) {return n *= random();}
    // [a, b)
    double uniform(double a, double b) {
        b -= a;
        return a += uniform(b);
    }
    
    ////////////////////
    // continuous
    
    // E = 1/lambda, V = 1/lambda^2
    double exponential(const double lambda=1.0) {
        return -std::log(random_oc()) / lambda;
    }
    
    // Scale-free: E = ?, V = ?
    double power(const double k=1.0, const double min=1.0) {
        return min*std::pow(random_oc(), -1.0 / k);
    }
    
    // E = mu, V = sigma^2
    double gauss(const double mu=0.0, const double sigma=1.0) {
        // Box-Muller method
        // generate two gaussian random numbers from 2 uniform (0,1] random numbers
        if ((flag_ =! flag_)) {
            t_ = std::sqrt(-2.0 * std::log(random_oc()));
            u_ = 2.0 * M_PI * random();
            return mu + sigma * t_ * std::cos(u_);
        } else {
            return mu + sigma * t_ * std::sin(u_);
        }
    }
    double normal(const double mu=0.0, const double sigma=1.0) {return gauss(mu, sigma);}
    
    
    ////////////////////
    // discrete
    
    // return true with probability p
    // E = p, V = p(1-p)
    bool bernoulli(const double p=0.5) {return p > random();}
    
    // The number of true in n trials with probability p
    // E = np, V = np(1-p)
    unsigned int binomial(const unsigned int n, const double p=0.5) {
        unsigned int cnt(0);
        for (unsigned int i=0; i<n; ++i) {if(bernoulli(p)) ++cnt;}
        return cnt;
    }
    
    // The expected number of occurrences in an unit of time/space
    // E = V = lambda
    unsigned int poisson(double lambda) {
        if (lambda < 256) {
            lambda = std::exp(-lambda);
            unsigned int k = 0;
            double p = 1.0;
            do {
                ++k;
                p *= random();
            } while (p > lambda);
            return --k;
        } else {
            const double sq = std::sqrt(2.0 * lambda);
            const double alxm = std::log(lambda);
            const double g = lambda * alxm - std::lgamma(lambda + 1.0);
            double y = 0.0;
            double em = 0.0;
            do {
                do {
                    y = std::tan(M_PI * random());
                    em = y;
                    em *= sq;
                    em += lambda;
                } while (em < 0.0);
                em = std::round(em);
                y *= y;
                y += 1.0;
                y *= 0.9;
                y *= std::exp(em * alxm - std::lgamma(em + 1.0) - g);
            } while (random() > y);
            return static_cast<unsigned int>(em);
        }
    }
    
    // The number of trials needed to get first true
    // E = (1-p)/p, V = (1-p)/p^2
    unsigned int geometric(const double p) {
        return static_cast<unsigned int>(std::log(random_oc()) / std::log(1.0 - p));
    }
    
    ////////////////////
    // algorithm
    
    template <class Iter>
    void shuffle(Iter begin_, Iter end_) {std::shuffle(begin_, end_, *this);}

    template <class T>
    void shuffle(T* x) {std::shuffle(begin(*x), end(*x), *this);}
    
    template <class Iter>
    Iter choice(Iter begin_, Iter end_) {
        std::advance(begin_, randrange(std::distance(begin_, end_)));
        return begin_;
    }
    
    template <class Container>
    void sample(Container* src, const size_t k) {
        size_t n(src->size());
        assert(k <= n);
        Container dst;
        dst.reserve(k);
        for (size_t i(0); i<k; ++i) {
            size_t j(randrange(n-i));
            dst.push_back((*src)[j]);
            (*src)[j] = (*src)[n-i-1];
        }
        src->swap(dst);
    }
    
    template <class Container>
    Container sample(Container src, const size_t k) {
        sample(&src, k);
        return src;
    }
    
    // faster than sample() above when k << n
    template <class Container>
    void sample_small(Container* src, const size_t k) {
        size_t n(src->size());
        assert(k <= n);
        std::set<size_t> selected_indices;
        Container dst;
        dst.reserve(k);
        for (size_t i(0); i<k; ++i) {
            size_t j(0);
            do {
                j = randrange(n);
            } while (!std::get<1>(selected_indices.insert(j)));
            dst.push_back((*src)[j]);
        }
        src->swap(dst);
    }
    
    template <class Container>
    Container sample_small(Container src, const size_t k) {
        sample_small(&src, k);
        return src;
    }
    
    // for std::shuffle()
    result_type operator()(const argument_type arg) {return randrange(arg);}
    
  private:
    unsigned int seed_;
    Generator generator_;
    
    // for Box-Muller in gauss()
    bool flag_;
    double t_;
    double u_;
};


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
////// Generators


class sfmt19937{
  public:
    typedef unsigned int result_type;
    typedef sfmt_t state_type;

    static constexpr result_type min() {return 0U;}
    static constexpr result_type max() {
        return std::numeric_limits<result_type>::max();
    }

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
    double canonical() {return (1.0 / max()) * operator()();}
    
    void seed(result_type s) {
        for (unsigned int i=0; i<4; ++i) {
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
        for (unsigned int i=0; i<4; ++i) {
            state_[i] = state[i];
        }
    }

  private:
    state_type state_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Global typedef and declaration

typedef Prandom<sfmt19937> Random;

namespace detail {
    template <class T> class Holder {
      public:
        T& operator()(void) {return instance_;}
      private:
        static T instance_;
    };
    template <class T> T Holder<T>::instance_(std::random_device{}());
} // namespace detail

inline Random& prandom() {
    return wtl::detail::Holder<Random>{}();
}

inline sfmt19937& sfmt() {
    return wtl::detail::Holder<sfmt19937>{}();
}

inline std::mt19937& mt() {
    return wtl::detail::Holder<std::mt19937>{}();
}

template<class Fn>
std::function<typename Fn::result_type ()> std_rng(Fn&& function) {
    return std::bind(function, std::ref(mt()));
}

template<class Fn>
std::function<typename Fn::result_type ()> std_rng(Fn& function) {
    return std::bind(function, std::ref(mt()));
}

template<class Fn>
std::function<typename Fn::result_type ()> rng(Fn&& function) {
    return std::bind(function, std::ref(prandom()));
}

template<class Fn>
std::function<typename Fn::result_type ()> rng(Fn& function) {
    return std::bind(function, std::ref(prandom()));
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* PRANDOM_HPP_ */

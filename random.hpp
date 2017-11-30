#pragma once
#ifndef WTL_RANDOM_HPP_
#define WTL_RANDOM_HPP_

#include <cstdint>
#include <limits>
#include <random>
#include <vector>
#include <unordered_set>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace {
    union bits64_t {
        uint64_t as_uint64_t;
        uint32_t as_uint32_t[2];
        double as_double;

        bits64_t(uint64_t x): as_uint64_t{x} {}
        bits64_t(uint32_t x, uint32_t y): as_uint32_t{x, y} {}

        // Use 52 bits to make double [0.0, 1.0)
        double as_canonical() const {
            bits64_t exponent_zero = (as_uint64_t >> 2) | 0x3ff0'0000'0000'0000u;
            return exponent_zero.as_double - 1.0;
        }
    };
}

class random_device_64 {
  public:
    typedef uint64_t result_type;
    static constexpr result_type min() {return 0u;}
    static constexpr result_type max() {return std::numeric_limits<result_type>::max();}

    result_type operator()() {
        return bits64_t(std_rd(), std_rd()).as_uint64_t;
    }
    double entropy() const noexcept {return std_rd.entropy();}
  private:
    std::random_device std_rd;
};

template <class URBG> inline
double generate_canonical(URBG& gen) {
    if (URBG::max() == std::numeric_limits<uint64_t>::max()) {
        return bits64_t(gen()).as_canonical();
    } else if (URBG::max() == std::numeric_limits<uint32_t>::max()) {
        return bits64_t(gen(), gen()).as_canonical();
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

template <class Container, class URBG> inline
std::vector<typename Container::value_type>
sample(const Container& src, const size_t k, URBG& engine) {
    const size_t n = src.size();
    if (100 * k < n) {return sample_floyd(src, k, engine);}
    else if (5 * k < n) {return sample_fisher(src, k, engine);}
    else {return sample_knuth(src, k, engine);}
}

//! Floyd's algorithm
//! fast if k << n
template <class Container, class URBG> inline
std::vector<typename Container::value_type>
sample_floyd(const Container& src, const size_t k, URBG& engine) {
    const size_t n = src.size();
    if (n < k) throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": n < k");
    std::unordered_set<size_t> existing_indices;
    std::vector<typename Container::value_type> dst;
    dst.reserve(k);
    std::uniform_int_distribution<size_t> uniform(0, n - 1);
    for (size_t upper = n - k; upper < n; ++upper) {
        size_t idx = std::uniform_int_distribution<size_t>(0, upper)(engine);
        if (existing_indices.insert(idx).second) {
            dst.push_back(src[idx]);
        } else {
            dst.push_back(src[upper]);
        }
    }
    return dst;
}

//! Fisher-Yates algorithm; Durstenfeld; Knuth's P
//! consistently fast; note that whole src is copied first
template <class Container, class URBG> inline
std::vector<typename Container::value_type>
sample_fisher(const Container& src, const size_t k, URBG& engine) {
    std::vector<typename Container::value_type> dst(std::begin(src), std::end(src));
    const size_t n = dst.size();
    if (n < k) throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": n < k");
    const size_t last = n - 1;
    for (size_t i=0; i<k; ++i) {
        std::swap(dst[std::uniform_int_distribution<size_t>(i, last)(engine)], dst[i]);
    }
    dst.resize(k);
    return dst;
}

//! Knuth's algorithm S
//! fast if k / n is large
//! The order is not random
template <class Container, class URBG> inline
std::vector<typename Container::value_type>
sample_knuth(const Container& src, const size_t k, URBG& engine) {
    const size_t n = src.size();
    if (n < k) throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": n < k");
    std::vector<typename Container::value_type> dst;
    dst.reserve(k);
    size_t i = 0;
    for (; i<k; ++i) {
        dst.push_back(src[i]);
    }
    std::uniform_int_distribution<size_t> uniform(0, k - 1);
    while (i < n) {
        if (std::bernoulli_distribution((1.0 / ++i) * k)(engine)) {
            dst[uniform(engine)] = src[i - 1];
        }
    }
    return dst;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

//! Pythonic engine object
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

inline Prandom<std::mt19937>& prandom() {
    static Prandom<std::mt19937> generator;
    return generator;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_RANDOM_HPP_ */

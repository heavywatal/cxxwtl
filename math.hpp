#pragma once
#ifndef WTL_MATH_HPP_
#define WTL_MATH_HPP_

#include <cmath>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// interger powers at compile time
template <class T>
constexpr T pow(T base, unsigned int exponent) noexcept {
    return exponent == 0 ? 1 : base * pow(base, --exponent);
}

// factorial at compile time
constexpr unsigned int factorial(unsigned int n) {
    return n ? (n * factorial(n - 1u)) : 1u;
}

constexpr unsigned int permut(unsigned int n, unsigned int r) {
    return r ? (n * permut(n - 1u, --r)) : 1u;
}

constexpr unsigned int choose(unsigned int n, unsigned int r) {
    return permut(n, r) / factorial(r);
}

template <class T>
inline unsigned int multinomial(const T& v) {
    unsigned int out = factorial(v.sum());
    for (const auto x: v) {
        out /= factorial(x);
    }
    return out;
}

inline unsigned int permut_runtime(unsigned int n, unsigned int r) {
    unsigned int answer = n;
    while (--r) {
        answer *= --n;
    }
    return answer;
}

inline unsigned int choose_runtime(unsigned int n, unsigned int r) {
    unsigned int numerator = n;
    unsigned int denominator = r;
    while (--r) {
        numerator *= --n;
        denominator *= r;
    }
    return numerator /= denominator;
}

// step function: sign, heaviside
template <class T> inline
constexpr int sign(const T& x) {return (x < 0) ? -1 : (x == 0) ? 0 : 1;}

template <class T> inline
constexpr int heaviside0(const T& x) {return (x <= 0) ? 0 : 1;}
template <class T> inline
constexpr int heaviside1(const T& x) {return (x < 0) ? 0 : 1;}
template <class T> inline
constexpr double heaviside1_2(const T& x) {return (x < 0) ? 0 : (x == 0) ? 0.5 : 1;}

template <class Ret, class Arg>
class Heaviside {
  public:
    explicit Heaviside(const Ret& a=0): a_(a) {}
    constexpr Ret operator()(const Arg& x) const {
        return (x < 0) ? 0 : (x == 0) ? a_ : 1;
    }
  private:
    const Ret a_;
};


// x(-∞,∞) -> y(0,1)
inline double sigmoid(const double& x, const double& gain=1.0) {
    return 1.0 / (1.0 + std::exp(-gain * x));
}
// (0, 1)
class Sigmoid {
  public:
    Sigmoid(const double& gain): gain_(gain) {}
    double operator()(const double& x) const {
        return 1.0 / (1.0 + std::exp(-gain_ * x));
    }
  private:
    const double gain_;
};

// (-1, 1)
class Tanh {
  public:
    Tanh(const double& a): a_(a) {}
    double operator()(const double& x) const {return std::tanh(a_ * x);}
  private:
    const double a_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_MATH_HPP_ */

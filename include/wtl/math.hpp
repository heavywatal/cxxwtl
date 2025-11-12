#pragma once
#ifndef WTL_MATH_HPP_
#define WTL_MATH_HPP_

#include <cmath>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// integer powers at compile time
template <class T, class Int>
constexpr T pow(T base, Int exponent) noexcept {
    return exponent == 0 ? 1 : base * pow(base, --exponent);
}

// factorial at compile time
template <class Int>
constexpr Int factorial(Int n) {
    return n ? (n * factorial(n - 1)) : 1;
}

template <class Int>
constexpr Int permut(Int n, Int r) {
    return r ? (n * permut(n - 1, --r)) : 1;
}

template <class Int>
constexpr Int choose(Int n, Int r) {
    return permut(n, r) / factorial(r);
}

template <class ValArray>
inline auto multinomial(const ValArray& v) {
    auto out = factorial(v.sum());
    for (const auto x: v) {
        out /= factorial(x);
    }
    return out;
}

template <class Int>
inline Int permut_runtime(Int n, Int r) {
    Int answer = n;
    while (--r) {
        answer *= --n;
    }
    return answer;
}

template <class Int>
inline Int choose_runtime(Int n, Int r) {
    Int numerator = n;
    Int denominator = r;
    while (--r) {
        numerator *= --n;
        denominator *= r;
    }
    return numerator /= denominator;
}

// step function: sign, heaviside
template <class T> inline
constexpr int sign(T x) {return (x < 0) ? -1 : (x == 0) ? 0 : 1;}

template <class T> inline
constexpr int heaviside0(T x) {return (x <= 0) ? 0 : 1;}
template <class T> inline
constexpr int heaviside1(T x) {return (x < 0) ? 0 : 1;}
template <class T> inline
constexpr double heaviside1_2(T x) {return (x < 0) ? 0 : (x == 0) ? 0.5 : 1;}

template <class Ret, class Arg>
class Heaviside {
  public:
    explicit Heaviside(Ret a=0): a_(a) {}
    constexpr Ret operator()(Arg x) const {
        return (x < 0) ? 0 : (x == 0) ? a_ : 1;
    }
  private:
    const Ret a_;
};


// x(-∞,∞) -> y(0,1)
inline double sigmoid(double x, double gain=1.0) {
    return 1.0 / (1.0 + std::exp(-gain * x));
}
// (0, 1)
class Sigmoid {
  public:
    Sigmoid(double gain): gain_(gain) {}
    double operator()(double x) const {
        return 1.0 / (1.0 + std::exp(-gain_ * x));
    }
  private:
    const double gain_;
};

// (-1, 1)
class Tanh {
  public:
    Tanh(double a): a_(a) {}
    double operator()(double x) const {return std::tanh(a_ * x);}
  private:
    const double a_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_MATH_HPP_ */

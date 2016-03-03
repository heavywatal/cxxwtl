// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef ALGORITHM_HPP_
#define ALGORITHM_HPP_

#include <cmath>

#include <map>
#include <utility>
#include <algorithm>
#include <functional>
#include <numeric>
#include <stdexcept>

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
    return n ? (n * factorial(n - 1)) : 1;
}

// sum
template <class Iter> inline
typename Iter::value_type sum(const Iter begin_, const Iter end_) {
    return std::accumulate(begin_, end_, typename Iter::value_type{});
}
template <class V> inline
typename V::value_type sum(const V& v) {return sum(begin(v), end(v));}

template <class Iter> inline
typename Iter::value_type kahan_sum(Iter begin_, const Iter end_) {
    typedef typename Iter::value_type T;
    T result = *begin_;
    T left_behind = 0;
    while (++begin_ != end_) {
        T load = (*begin_) + left_behind;
        left_behind = load - ((result + load) - result);
        result += load;
    }
    return result;
}


// product
template <class Iter> inline
typename Iter::value_type prod(const Iter begin_, const Iter end_) {
    typedef typename Iter::value_type T;
    return std::accumulate(begin_, end_, T{1}, std::multiplies<T>{});
}
template <class V> inline
typename V::value_type prod(const V& v) {return prod(begin(v), end(v));}


// arithmetic (additive) mean
template <class Iter> inline
double mean(const Iter begin_, const Iter end_) {
    double x = sum(begin_, end_);
    return x /= std::distance(begin_, end_);
}
template <class V> inline
double mean(const V& v) {
    double x = sum(v);
    return x /= v.size();
}


// geographic (multiplicative) mean
template <class Iter> inline
double gmean(const Iter begin_, const Iter end_) {
    return std::pow(prod(begin_, end_), 1.0 / std::distance(begin_, end_));
}
template <class V> inline
double gmean(const V& v) {
    return std::pow(prod(v), 1.0 / v.size());
}


// median
template <class RandIter> inline
double median(const RandIter begin_, const RandIter end_) {
    const size_t n = std::distance(begin_, end_);
    const RandIter mid = begin_ + (n >> 1); // larger one if n is even
    std::nth_element(begin_, mid, end_);
    if (n&1) { // n is odd if the first bit is 1
        return *mid;
    } else {
        std::nth_element(begin_, mid - 1, end_);
        return 0.5 * (*(mid - 1) + *(mid));
    }
}
template <class V> inline
double median(V* v) {return median(v->begin(), v->end());}


// mode: the value that appears most often
// Sort the container before applying this
template <class Iter> inline
typename Iter::value_type mode(Iter begin_, const Iter end_) {
    size_t score = 0;
    Iter resiter;
    while (begin_!=end_) {
        Iter bound(std::upper_bound(begin_, end_, *begin_));
        const size_t d(std::distance(begin_, bound));
        if (d > score) {
            score = d;
            resiter = begin_;
        }
        begin_ = bound;
    }
    return *resiter;
}
template <class V> inline
typename V::value_type mode(const V& v) {return mode(begin(v), end(v));}


// deviation squares
template <class Iter> inline
typename Iter::value_type devsq(Iter begin_, const Iter end_, const typename Iter::value_type& theta=0) {
    typename Iter::value_type result = 0;
    for (; begin_!=end_; ++begin_) {
        auto tmp = *begin_;
        tmp -= theta;
        tmp *= tmp;
        result += tmp;
    }
    return result;
}
template <class V> inline
typename V::value_type devsq(const V& v, const typename V::value_type& theta=0) {
    return devsq(begin(v), end(v), theta);
}


// mean square deviation
template <class Iter> inline
double qmean(const Iter begin_, const Iter end_, const typename Iter::value_type& theta=0) {
    double x = devsq(begin_, end_, theta);
    return x /= std::distance(begin_, end_);
}
template <class V> inline
double qmean(const V& v, const typename V::value_type& theta=0) {
    double x = devsq(v, theta);
    return x /= v.size();
}


// root mean square deviation (RMSD)
template <class Iter> inline
double rmsd(const Iter begin_, const Iter end_, const typename Iter::value_type& theta=0) {
    return std::sqrt(qmean(begin_, end_, theta));
}
template <class V> inline
double rmsd(const V& v, const typename V::value_type& theta=0) {
    return rmsd(begin(v), end(v), theta);
}


// variance
template <class Iter> inline
double var(const Iter begin_, const Iter end_, const bool unbiased=true) {
    size_t denom = std::distance(begin_, end_);
    denom -= static_cast<size_t>(unbiased);
    return devsq(begin_, end_, mean(begin_, end_)) / denom;
}
template <class V> inline
double var(const V& v, const bool unbiased=true) {
    size_t denom = v.size();
    denom -= static_cast<size_t>(unbiased);
    return devsq(v, mean(v)) / denom;
}

template <class Iter> inline
double var_fast(Iter begin_, const Iter end_, const bool unbiased=true) {
    size_t n = std::distance(begin_, end_);
    n -= static_cast<size_t>(unbiased);
    typename Iter::value_type sq(0), s(0);
    // Be careful not to overflow if value_type is int
    for (; begin_!=end_; ++begin_) {
        sq += (*begin_) * (*begin_);
        s += *begin_;
    }
    return (sq - (1.0 / n) * s * s) / n;
}

template <class Iter> inline
double var_once(Iter begin_, const Iter end_, const bool unbiased=true) {
    size_t n = 0;
    double wmean = 0.0;
    double sqsum = 0.0;
    // Only one loop, but many calculation steps -> slow
    for (; begin_!=end_; ++begin_) {
        double d = *begin_;
        d -= wmean;
        wmean += d / (++n);
        sqsum += (n - 1) * d * d / n;
    }
    n -= static_cast<int>(unbiased);
    return sqsum /= n;
}


// standard deviation
template <class Iter> inline
double sd(const Iter begin_, const Iter end_, const bool unbiased=true) {
    return std::sqrt(var(begin_, end_, unbiased));
}

template <class V> inline
double sd(const V& v, const bool unbiased=true) {
    return std::sqrt(var(v, unbiased));
}


// standard error of the mean (fpc: finite population correction)
template <class Iter> inline
double sem(const Iter begin_, const Iter end_, const size_t& N=0) {
    const size_t n(std::distance(begin_, end_));
    double fpc(1.0);
    if (N) {
        if (N < n) {throw std::range_error("N<n in sem()");}
        fpc = (N - n) * (1.0 / (N - 1));
    }
    return std::sqrt(fpc * var(begin_, end_) / n);
}
template <class V> inline
double sem(const V& v, const size_t& N=0) {return sem(begin(v), end(v), N);}


// coveriance
template <class Iter1, class Iter2> inline
double cov(const Iter1 begin1, const Iter1 end1, const Iter2 begin2, const Iter2 end2, const bool unbiased=true) {
    const double mean_x = mean(begin1, end1);
    const double mean_y = mean(begin2, end2);
    double x = std::inner_product(begin1, end1, begin2, 0.0, std::plus<double>(),
        [&](typename Iter1::value_type x, typename Iter2::value_type y) {
            x -= mean_x;
            y -= mean_y;
            return x *= y;
        }
    );
    return x /= (std::distance(begin1, end1) - static_cast<int>(unbiased));
}

template <class V, class U> inline
double cov(const V& v, const U& u, const bool unbiased=true) {
    return cov(v.cbegin(), v.cend(), u.cbegin(), u.cend(), unbiased);
}

// rank
template <class Iter> inline
std::vector<double> rank(const Iter begin_, const Iter end_) {
    typedef typename Iter::value_type T;
    std::map<T, unsigned int> mtu;
    for (auto it=begin_; it!=end_; ++it) {
        ++mtu[*it];
    }
    std::map<T, double> mtd;
    unsigned int i = 0;
    for (auto it=mtu.cbegin(); it!=mtu.cend(); ++it) {
        double r = 0.0;
        const unsigned int n(std::get<1>(*it));
        for (unsigned int j=0; j<n; ++j) {
            r += ++i;
        }
        r /= n;
        mtd[std::get<0>(*it)] = r;
    }
    std::vector<double> dst;
    dst.reserve(i);
    for (auto it=begin_; it!=end_; ++it) {
        dst.push_back(mtd[*it]);
    }
    return dst;
}

template <class V> inline
std::vector<double> rank(const V& v) {
    return rank(v.cbegin(), v.cend());
}

// Pearson product-moment correlation coefficient

//template <class Iter1, class Iter2> inline
//double cor_pearson_simple(const Iter1 begin1, const Iter1 end1, const Iter2 begin2, const Iter2 end2) {
//    return cov(begin1, end1, begin2, end2) / sd(begin1, end1) / sd(begin2, end2);
//}

template <class Iter1, class Iter2> inline
double cor_pearson(Iter1 begin1, const Iter1 end1, Iter2 begin2, const Iter2 end2) {
    double mean1 = mean(begin1, end1);
    double mean2 = mean(begin2, end2);
    double sum1(0), sum2(0), sum12(0);
    for (; begin1!=end1; ++begin1, ++begin2) {
        double div1(*begin1), div2(*begin2);
        div1 -= mean1;
        div2 -= mean2;
        sum1 += div1 * div1;
        sum2 += div2 * div2;
        sum12 += div1 * div2;
    }
    sum12 /= std::sqrt(sum1);
    sum12 /= std::sqrt(sum2);
    return sum12;
}

template <class V, class U> inline
double cor_pearson(const V& v, const U& u) {
    return cor_pearson(v.cbegin(), v.cend(), u.cbegin(), u.cend());
}

// Spearman rank correlation coefficient

template <class Iter1, class Iter2> inline
double cor_spearman(Iter1 begin1, const Iter1 end1, Iter2 begin2, const Iter2 end2) {
    return cor_pearson(rank(begin1, end1), rank(begin2, end2));
}

template <class V1, class V2> inline
double cor_spearman(const V1& v1, const V2& v2) {
    return cor_spearman(v1.cbegin(), v1.cend(), v2.cbegin(), v2.cend());
}

// squared euclid distance between 2 vectors
template <class Iter1, class Iter2> inline
typename Iter1::value_type squared_euclidean(const Iter1 begin1, const Iter1 end1, const Iter2 begin2) {
    typedef typename Iter1::value_type T;
    return std::inner_product(begin1, end1, begin2, 0.0, std::plus<T>(),
        [](T x, const T y) {
            x -= y;
            return x *= x;
        }
    );
}

template <class V, class U> inline
typename V::value_type squared_euclidean(const V& v, const U& u) {
    return squared_euclidean(begin(v), end(v), begin(u));
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// Simpson Diversity D
template <class V> inline
double simpson_diversity(const V& v) {
    const double square1_N = pow<2>(1.0 / sum(v));
    double d = 0.0;
    for (const auto& x: v) {
        auto tmp = square1_N;
        tmp *= x;
        tmp *= x;
        d += tmp;
    }
    return d;
}

// Shannon's diversity H'
template <class V> inline
double shannon_diversity(const V& v) {
    if (v.size() < 2) {return 0.0;}
    typename V::value_type n = 0;
    double nlogn = 0.0;
    for (const auto& n_i: v) {
        if (n_i > 0) {
            n += n_i;
            double dn = static_cast<double>(n_i);
            dn *= std::log(dn);
            nlogn += dn;
        }
    }
    return std::log(n) - nlogn / n;
}


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class V, class U> inline
V subscript(const V& seq, const U& indices) {
    V subset;
    subset.reserve(indices.size());
    for (const auto i: indices) {
        subset.push_back(seq[i]);
    }
    return subset;
}

template <class T> inline
typename std::vector<typename std::remove_pointer<typename T::value_type>::type>
dereference(const T& src) {
    typename std::vector<typename std::remove_pointer<typename T::value_type>::type> dst;
    dst.reserve(src.size());
    for (const auto p: src) {
        dst.push_back(*p);
    }
    return dst;
}

template <class Iter, class Distance> inline
Iter advance_return(Iter i, Distance d) {
    std::advance(i, d); return i;
}


// for vector and deque
template <class V> inline
typename V::iterator sort_unique_erase(V* v) {
    std::sort(begin(*v), end(*v));
    return v->erase(std::unique(begin(*v), end(*v)), end(*v));
}

template <class Matrix> inline
void transpose(Matrix* A) {
    const size_t nrow = A->size();
    const size_t ncol = A->begin()->size();
    Matrix tmp(ncol, typename Matrix::value_type(nrow));
    for (size_t row=0; row<nrow; ++row) {
        for (size_t col=0; col<ncol; ++col) {
            tmp[col][row] = A->operator[](row)[col];
        }
    }
    A->swap(tmp);
}

// ceilint of integer division
template <class T> inline
T ceil_int_div(T lhs, const T rhs) {
    lhs += rhs;
    return --lhs /= rhs;
}


// split a sequence into the equally sized pieces
template <class V> inline
std::vector<V> chunk(const V& src, const size_t unit) {
    std::vector<V> dst(ceil_int_div(src.size(), unit));
    size_t i = 0;
    for (auto it=begin(src); it!=end(src); ++it) {
        if (dst[i].size()==unit) {++i;}
        dst[i].push_back(*it);
    }
    return dst;
}


/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// easy equal, sign equal, sign less

// true if signs are the same or both 0
template <class T> struct alike_sign {public:
    constexpr bool operator()(const T& x, const T& y) const {
        return (x * y > T()) || (x == y);
    }
};

// true if std::less(x, y) and signs are different
template <class T> struct less_sign {public:
    constexpr bool operator()(const T& x, const T& y) const {
        return (x < y) && (x * y <= T());
    }
};

template <class V, class Fn> inline
bool equal(const V& v, const V& u, Fn&& fn) {
    return std::equal(begin(v), end(v), begin(u), fn);
}

template <class V, class Fn> inline
bool equal_matrix(const V& v, const V& u, Fn&& fn) {
    typedef typename V::value_type T;
    return wtl::equal(v, u,
                [&fn](const T& x, const T& y) {
                    return wtl::equal(x, y, fn);
                }
           );
}

template <class T> inline
bool equal_sign(const T& x, const T& y) {
        return wtl::equal(x, y, wtl::alike_sign<typename T::value_type>{});
}

template <class T> inline
bool equal_matrix_sign(const T& x, const T& y) {
    return wtl::equal_matrix(x, y, wtl::alike_sign<typename T::value_type::value_type>{});
}

template <class T> inline
bool lexico_sign(const T& x, const T& y) {
    return std::lexicographical_compare(begin(x), end(x), begin(y), end(y),
                    wtl::less_sign<typename T::value_type>{});
}

template <class T> inline
bool lexico_matrix_sign(const T& x, const T& y) {
    return std::lexicographical_compare(begin(x), end(x), begin(y), end(y),
                    wtl::lexico_sign<typename T::value_type>);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// <functional> for std::pair

template <class P, int N> struct plus
: public std::binary_function<const typename std::tuple_element<N, P>::type&, const P&, typename std::tuple_element<N, P>::type>{
    typedef typename std::tuple_element<N, P>::type T;
    T operator()(const T& x, const P& y) const {return x + std::get<N>(y);}
};

template <class P, int N> struct minus
: public std::binary_function<const typename std::tuple_element<N, P>::type&, const P&, typename std::tuple_element<N, P>::type>{
    typedef typename std::tuple_element<N, P>::type T;
    T operator()(const T& x, const P& y) const {return x - std::get<N>(y);}
};

template <class P, int N> struct multiplies
: public std::binary_function<const typename std::tuple_element<N, P>::type&, const P&, typename std::tuple_element<N, P>::type>{
    typedef typename std::tuple_element<N, P>::type T;
    T operator()(const T& x, const P& y) const {return x * std::get<N>(y);}
};

template <class P, int N> struct divides
: public std::binary_function<const typename std::tuple_element<N, P>::type&, const P&, typename std::tuple_element<N, P>::type>{
    typedef typename std::tuple_element<N, P>::type T;
    T operator()(const T& x, const P& y) const {return x / std::get<N>(y);}
};

template <class P, int N> struct equal_to
: public std::binary_function<const P&, const P&, bool>{
    bool operator()(const P& lhs, const P& rhs) const {
        return std::get<N>(lhs) == std::get<N>(rhs);
    }
};

template <class P, int N> struct less
: public std::binary_function<const P&, const P&, bool>{
    bool operator()(const P& lhs, const P& rhs) const {
        return std::get<N>(lhs) < std::get<N>(rhs);
    }
};

template <int N, class Iter> inline
typename std::tuple_element<N, typename Iter::value_type>::type
sum(const Iter begin_, const Iter end_) {
    typedef typename Iter::value_type pair_t;
    typedef typename std::tuple_element<N, pair_t>::trait_t T;
    return std::accumulate(begin_, end_, T(0), wtl::plus<pair_t, N>());
}

template <int N, class Iter> inline
double mean(const Iter begin_, const Iter end_) {
    double x(sum<N>(begin_, end_));
    return x /= std::distance(begin_, end_);
}

template <int N, class Iter1, class Iter2> inline
bool equal(const Iter1 begin1, const Iter1 end1, const Iter2 begin2) {
    return std::inner_product(begin1, end1, begin2, true, std::logical_and<bool>(), wtl::equal_to<typename Iter1::value_type,N>());
}

template <int N, class Iter> inline
Iter max_element(const Iter begin_, const Iter end_) {
    return std::max_element(begin_, end_, wtl::less<typename Iter::value_type,N>());
}

template <int N, class IterMap, class IterVec> inline
IterVec copy(const IterMap begin_, const IterMap end_, IterVec dst) {
    return std::transform(begin_, end_, dst,
        [](const typename IterMap::value_type& x) {
            return std::get<N>(x);
        });
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// std::map

template <size_t I, class T> inline
std::vector<typename std::remove_const<typename std::tuple_element<I, typename T::value_type>::type>::type>
unmap(const T& map_) {
    std::vector<typename std::remove_const<typename std::tuple_element<I, typename T::value_type>::type>::type> output;
    output.reserve(map_.size());
    for (const auto& item: map_) {
        output.push_back(std::get<I>(item));
    }
    return output;
}

template <class T> inline
std::vector<typename std::remove_const<typename std::tuple_element<0, typename T::value_type>::type>::type>
keys(const T& map_) {
    return unmap<0>(map_);
}

template <class T> inline
std::vector<typename std::remove_const<typename std::tuple_element<1, typename T::value_type>::type>::type>
values(const T& map_) {
    return unmap<1>(map_);
}

template <class Map> inline
std::vector<typename Map::mapped_type> expand_counter(const Map& src) {
    std::vector<typename Map::mapped_type> dst;
    dst.reserve(src.size());
    for (const auto& item: src) {
        dst->insert(dst->end(), std::get<1>(item), std::get<0>(item));
    }
    return dst;
}

template <class DstCountMap, class SrcCountMap> inline
void recount(DstCountMap* dst, const SrcCountMap& src) {
    for (const auto& item: src) {
        (*dst)[std::get<0>(item)] += std::get<1>(item);
    }
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// nPr
inline unsigned int permutation(unsigned int n, unsigned int r) {
    unsigned int answer = n;
    while (--r) {
        answer *= --n;
    }
    return answer;
}

// nCr
inline unsigned int combination(unsigned int n, unsigned int r) {
    unsigned int numerator = n;
    unsigned int denominator = r;
    while (--r) {
        numerator *= --n;
        denominator *= r;
    }
    return numerator /= denominator;
}


template <class InputIter, class OutputIter, class BinaryOperator> inline
OutputIter pairwise_transform(const InputIter begin_, const InputIter end_, OutputIter result, BinaryOperator op) {
    for (auto it1=begin_; it1!=end_; ++it1) {
        for (auto it2=it1; ++it2!=end_; ++result) {
            *result = op(*it1, *it2);
        }
    }
    return result;
}

template <class Iter, class BinaryOperator> inline
std::vector<typename BinaryOperator::result_type> pairwise_transform(const Iter begin_, const Iter end_, BinaryOperator op) {
    const size_t n = std::distance(begin_, end_);
    std::vector<typename BinaryOperator::result_type> result(combination(n, 2));
    pairwise_transform(begin_, end_, begin(result), op);
    return result;
}

template <class Iter> inline
std::pair<Iter, Iter> nth_pair(const Iter begin_, const Iter end_, const size_t n) {
    size_t i = 0;
    for (auto it1=begin_; it1!=end_; ++it1) {
        for (auto it2=it1; ++it2!=end_; ++i) {
            if (i == n) {return {it1, it2};}
        }
    }
    return {end_, end_};
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// 2^n?
inline constexpr bool is_power_of_two(const int& num) {
    return num && !(num & (num - 1));
}

// xy; sign = (+ - - -)
template <class T> inline
T and_mul(T x, const T y) {
    if (x < 0 && y < 0) {
        x *= -1;
    }
    x *= y;
    return x;
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
class Heaviside: public std::unary_function<Arg, Ret>{
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
class Sigmoid: public std::unary_function<double, double> {
  public:
    Sigmoid(const double& gain): gain_(gain) {}
    double operator()(const double& x) const {
        return 1.0 / (1.0 + std::exp(-gain_ * x));
    }
  private:
    const double gain_;
};

// (-1, 1)
class Tanh: public std::unary_function<double, double> {
  public:
    Tanh(const double& a): a_(a) {}
    double operator()(const double& x) const {return std::tanh(a_ * x);}
  private:
    const double a_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// numerical integration

template <class Func> inline
double integrate_trapezoid(Func func, const double lower, const double upper, const size_t precision=100) {
    const double step = (upper - lower) / precision;
    double result = func(lower);
    result += func(upper);
    result *= 0.5;
    for (double x=lower + step; x<upper; x+=step) {
        result += func(x);
    }
    return result *= step;
}

template <class Func> inline
double integrate_midpoint(Func func, const double lower, const double upper, const size_t precision=100) {
    const double step = (upper - lower) / precision;
    double result = 0.0;
    for (double x=lower + 0.5 * step; x<upper; x+=step) {
        result += func(x);
    }
    return result *= step;
}

template <class Func> inline
double integrate_simpson(Func func, const double lower, const double upper, const size_t precision=100) {
    const double step = (upper - lower) / precision;
    const double double_step = step * 2.0;
    double result_odd = 0.0;
    for (double x=lower + step; x<upper; x+=double_step) {
        result_odd += func(x);
    }
    result_odd *= 4.0;
    double result_even = 0.0;
    for (double x=lower + double_step; x<upper; x+=double_step) {
        result_even += func(x);
    }
    result_even *= 2.0;
    double result = func(lower);
    result += result_odd;
    result += result_even;
    result += func(upper);
    result /= 3.0;
    return result *= step;
}

template <class Func> inline
double integrate(Func func, const double lower, const double upper, const size_t precision=100) {
    return integrate_simpson(func, lower, upper, precision);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* ALGORITHM_HPP_ */

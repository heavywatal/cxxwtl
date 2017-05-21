// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_NUMERIC_HPP_
#define WTL_NUMERIC_HPP_

#include <cmath>

#include <numeric>
#include <limits>
#include <vector>
#include <valarray>
#include <map>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class T> inline
bool approx(const T x, const T y) {
    return std::abs(x - y) <
           std::numeric_limits<T>::epsilon() * (std::abs(x) + std::abs(y));
}

template <class T>
class Approx {
  public:
    Approx(const T value): value_(value) {}
    bool operator()(const T x) const {
        return approx<T>(x, value_);
    }
  private:
    const T value_;
};

template <class T> inline
Approx<T> approx(const T value) {return Approx<T>(value);}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// for std::valarray

// Generage valarray with equally spaced values
inline std::valarray<double>
lin_spaced(const size_t size, const double low=0.0, const double high=1.0) {
    const double step = (high - low) / (size - 1);
    std::valarray<double> x(step, size);
    for (size_t i=0; i<size; ++i) {
        x[i] *= i;
    }
    return x += low;
}

// Imperfect but convenient rounding
inline std::valarray<double>
round(const std::valarray<double>& x, const double precision=1) {
    return (x * precision).apply(std::round) / precision;
}

template <class T> inline std::valarray<T>
row_sums(const std::vector<std::valarray<T>>& matrix) {
    const size_t nrow = matrix.size();
    std::valarray<T> sums(nrow);
    for (size_t i=0; i<nrow; ++i) {
        sums[i] = matrix[i].sum();
    }
    return sums;
}

template <class T> inline std::valarray<T>
col_sums(const std::vector<std::valarray<T>>& matrix) {
    const size_t ncol = matrix.at(0).size();
    std::valarray<T> sums(ncol);
    for (const auto& row: matrix) {
        sums += row;
    }
    return sums;
}

template <class T> inline std::vector<std::valarray<T>>
filter(const std::vector<std::valarray<T>>& matrix, const std::valarray<bool>& binary) {
    std::vector<std::valarray<T>> filtered;
    filtered.reserve(binary.sum());
    const size_t nrow = matrix.size();
    for (size_t i=0; i<nrow; ++i) {
        if (binary[i]) filtered.push_back(matrix[i]);
    }
    return filtered;
}

template <class T, class U> inline
std::valarray<T>
cast(const std::valarray<U>& x) {
    const size_t n = x.size();
    std::valarray<T> y(n);
    for (size_t i=0; i<n; ++i) {
        y[i] = static_cast<T>(x[i]);
    }
    return y;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// for std::vector

// Generate integer vector with increasing values
template <class T=size_t> inline
typename std::vector<T> seq_len(const size_t size, const T init=0) {
    typename std::vector<T> v(size);
    std::iota(std::begin(v), std::end(v), init);
    return v;
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


// geometric (multiplicative) mean
template <class Iter> inline
double geomean(const Iter begin_, const Iter end_) {
    return std::pow(prod(begin_, end_), 1.0 / std::distance(begin_, end_));
}
template <class V> inline
double geomean(const V& v) {
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

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// for pairs

template <int N, class Iter> inline
typename std::tuple_element<N, typename Iter::value_type>::type
sum(const Iter begin_, const Iter end_) {
    typedef typename Iter::value_type pair_t;
    typedef typename std::tuple_element<N, pair_t>::trait_t T;
    return std::accumulate(begin_, end_, T(0),
        [](const T& x, const pair_t& p){return x + std::get<N>(p);});
}

template <int N, class Iter> inline
double mean(const Iter begin_, const Iter end_) {
    double x(sum<N>(begin_, end_));
    return x /= std::distance(begin_, end_);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
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
    const double square1_N = pow(1.0 / sum(v), 2);
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

template <class Matrix> inline
Matrix transpose(const Matrix& A) {
    const size_t nrow = A.size();
    if (nrow == 0) return A;
    const size_t ncol = A[0].size();
    Matrix out(ncol, typename Matrix::value_type(nrow));
    for (size_t row=0; row<nrow; ++row) {
        for (size_t col=0; col<ncol; ++col) {
            out[col][row] = A[row][col];
        }
    }
    return out;
}

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

#endif /* WTL_NUMERIC_HPP_ */

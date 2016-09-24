// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_ALGORITHM_HPP_
#define WTL_ALGORITHM_HPP_

#include <cmath>

#include <utility>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include "numeric.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
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
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_ALGORITHM_HPP_ */

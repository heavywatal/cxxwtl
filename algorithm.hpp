#pragma once
#ifndef WTL_ALGORITHM_HPP_
#define WTL_ALGORITHM_HPP_

#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <stdexcept>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class T> inline
size_t bisect_left(const std::vector<T>& array, const T val) {
    return std::lower_bound(array.begin(), array.end(), val) - array.begin();
}

template <class T> inline
size_t bisect_right(const std::vector<T>& array, const T val) {
    return std::upper_bound(array.begin(), array.end(), val) - array.begin();
}

template <class T> inline
size_t bisect(const std::vector<T>& array, const T val) {
    return bisect_right(array, val);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template<class T> inline
void rstrip(std::vector<T>* seq, const T& value=T(0)) {
    auto rit = std::find_if(seq->rbegin(), seq->rend(),
                           [&](const T& x){return x != value;});
    seq->resize(seq->rend() - rit);
}

template<class T> inline
std::vector<T> rstrip(std::vector<T> seq, const T& value=T(0)) {
    rstrip(&seq, value);
    return seq;
}

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

// ceiling of integer division
template <class T> inline constexpr
T ceil_int_div(T lhs, const T rhs) {
    --lhs /= rhs;
    return ++lhs;
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
// std::map

template <class Key, class Value> inline
std::map<Key, Value>
map(const std::vector<Key>& keys, const std::vector<Value>& values) {
    if (keys.size() != values.size()) {
        throw std::runtime_error("keys.size() != values.size()");
    }
    std::map<Key, Value> output;
    for (size_t i=0; i<keys.size(); ++i) {
        output[keys[i]] = values[i];
    }
    return output;
}

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
    const size_t combinations = n * (n - 1) / 2;
    std::vector<typename BinaryOperator::result_type> result(combinations);
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

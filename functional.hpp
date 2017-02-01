// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_FUNCTIONAL_HPP_
#define WTL_FUNCTIONAL_HPP_

#include <utility>
#include <functional>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

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

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// for std::pair

template <class P, int N> struct plus {
    typedef typename std::tuple_element<N, P>::type T;
    T operator()(const T& x, const P& y) const {return x + std::get<N>(y);}
};

template <class P, int N> struct minus {
    typedef typename std::tuple_element<N, P>::type T;
    T operator()(const T& x, const P& y) const {return x - std::get<N>(y);}
};

template <class P, int N> struct multiplies {
    typedef typename std::tuple_element<N, P>::type T;
    T operator()(const T& x, const P& y) const {return x * std::get<N>(y);}
};

template <class P, int N> struct divides {
    typedef typename std::tuple_element<N, P>::type T;
    T operator()(const T& x, const P& y) const {return x / std::get<N>(y);}
};

template <class P, int N> struct equal_to {
    bool operator()(const P& lhs, const P& rhs) const {
        return std::get<N>(lhs) == std::get<N>(rhs);
    }
};

template <class P, int N> struct less {
    bool operator()(const P& lhs, const P& rhs) const {
        return std::get<N>(lhs) < std::get<N>(rhs);
    }
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_FUNCTIONAL_HPP_ */

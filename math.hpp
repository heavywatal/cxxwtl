// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_MATH_HPP_
#define WTL_MATH_HPP_

#include <cmath>

#include <boost/math/distributions/normal.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace bmath = boost::math;

inline double normal_cdf(const double x, const double mean, const double sd) {
    if (sd == 0.0) {return 0.0;}
    return bmath::cdf(bmath::normal(mean, sd), x);
}

inline double normal_ccdf(const double x, const double mean, const double sd) {
    if (sd == 0.0) {return 0.0;}
    return bmath::cdf(bmath::complement(bmath::normal(mean, sd), x));
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_MATH_HPP_ */

#pragma once
#ifndef WTL_BMATH_HPP_
#define WTL_BMATH_HPP_

#include <boost/math/distributions/normal.hpp>
namespace bmath = boost::math;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

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

#endif /* WTL_BMATH_HPP_ */

#pragma once
#ifndef WTL_RESOURCE_HPP_
#define WTL_RESOURCE_HPP_

#include "signed.hpp"

#include <sys/time.h>
#include <sys/resource.h>
#include <algorithm>
#include <ratio>
#include <vector>
#include <ostream>

namespace wtl {

namespace detail {

template <class TimePrefix=std::micro>
struct TimeVal {
    explicit TimeVal(const timeval& tv) noexcept
    : sec(tv.tv_sec), usec(tv.tv_usec) {}
    long count() const noexcept {
        using FromMicro = std::ratio_divide<TimePrefix, std::micro>;
        return sec * TimePrefix::den / TimePrefix::num
               + usec * FromMicro::den / FromMicro::num;
    }
    operator long() const noexcept {return count();}
    const long sec;
    const long usec;
};

template <class T>
inline long unit(const timeval& tv) noexcept {
    return TimeVal<T>(tv).count();
}

template <class T>
inline long unit(long x) noexcept {
    if constexpr (std::is_same_v<T, std::kilo>) {
#ifdef __APPLE__
        return x >> 10;
#endif
    }
    else if constexpr (std::is_same_v<T, std::mega>) {
        return unit<std::kilo>(x) >> 10;
    }
    else if constexpr (std::is_same_v<T, std::giga>) {
        return unit<std::kilo>(x) >> 20;
    }
    return x;
}

template <class T> inline
T median(std::vector<T>* v) {
    const auto i = ssize(*v) / 2;
    std::nth_element(v->begin(), v->begin() + i, v->end());
    return at(*v, i);
}

} // namespace detail

inline rusage getrusage(int who = RUSAGE_SELF) {
    rusage ru;
    getrusage(who, &ru);
    return ru;
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF>
struct ResourceUsage {
    ResourceUsage(const rusage& ru = getrusage(Who)) noexcept
    : utime(detail::unit<TimePrefix>(ru.ru_utime)),
      stime(detail::unit<TimePrefix>(ru.ru_stime)),
      maxrss(detail::unit<MemoryPrefix>(ru.ru_maxrss)) {}
    ResourceUsage(const long u, const long s, const long m) noexcept
    : utime(u), stime(s), maxrss(m) {}
    ResourceUsage(const ResourceUsage& other) noexcept {
        const rusage ru = getrusage(Who);
        utime = detail::unit<TimePrefix>(ru.ru_utime) - other.utime;
        stime = detail::unit<TimePrefix>(ru.ru_stime) - other.stime;
        maxrss = detail::unit<MemoryPrefix>(ru.ru_maxrss) - other.maxrss;
    }
    ResourceUsage(ResourceUsage&&) = default;
    std::ostream& write(std::ostream& ost) const {
        return ost << utime << "\t" << stime << "\t" << maxrss;
    }
    constexpr static const char* header = "utime\tstime\tmaxrss";
    long utime;
    long stime;
    long maxrss;
};

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF>
inline ResourceUsage<TimePrefix, MemoryPrefix, Who>
getrusage() {
    return ResourceUsage<TimePrefix, MemoryPrefix, Who>();
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF, class Fn>
inline ResourceUsage<TimePrefix, MemoryPrefix, Who>
diff_rusage(Fn&& fun) {
    const auto start = ResourceUsage<TimePrefix, MemoryPrefix, Who>();
    fun();
    return ResourceUsage<TimePrefix, MemoryPrefix, Who>(start);
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF, class Fn>
inline ResourceUsage<TimePrefix, MemoryPrefix, Who>
diff_rusage(Fn&& fun, int times) {
    std::vector<long> utime;
    std::vector<long> stime;
    std::vector<long> maxrss;
    reserve(utime, times);
    reserve(stime, times);
    reserve(maxrss, times);
    for (int i = 0; i < times; ++i) {
        const auto ru = diff_rusage<TimePrefix, MemoryPrefix, Who>(std::forward<Fn>(fun));
        utime.push_back(ru.utime);
        stime.push_back(ru.stime);
        maxrss.push_back(ru.maxrss);
    }
    return ResourceUsage<TimePrefix, MemoryPrefix, Who>(
        detail::median(&utime), detail::median(&stime), detail::median(&maxrss)
    );
}

template <class TimePrefix, class MemoryPrefix, int Who=RUSAGE_SELF>
std::ostream& operator<<(std::ostream& ost, const ResourceUsage<TimePrefix, MemoryPrefix, Who>& x) {
    return x.write(ost);
}

constexpr const char* rusage_header() {return ResourceUsage<>::header;}

} // namespace wtl

#endif // WTL_RESOURCE_HPP_

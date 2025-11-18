#pragma once
#ifndef WTL_RESOURCE_HPP_
#define WTL_RESOURCE_HPP_

#include "signed.hpp"

#ifdef _WIN32
  constexpr int RUSAGE_SELF = 0;
  constexpr int RUSAGE_CHILDREN = -1;
  #include <winsock2.h> // timeval
#else
  #include <sys/resource.h>
#endif
#include <algorithm>
#include <ratio>
#include <tuple>
#include <utility>
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

template<class... T1, class... T2, std::size_t... I>
constexpr auto minus_impl(const std::tuple<T1...>& x1, const std::tuple<T2...>& x2, std::index_sequence<I...>) {
    return std::make_tuple(std::get<I>(x1) - std::get<I>(x2)...);
}

template<class... T1, class... T2>
constexpr auto minus(const std::tuple<T1...>& x1, const std::tuple<T2...>& x2) {
    static_assert(sizeof...(T1) == sizeof...(T2));
    return minus_impl(x1, x2, std::make_index_sequence<sizeof...(T1)>{});
}

template <class T> inline
T median(std::vector<T>* v) {
    const auto i = ssize(*v) / 2;
    std::nth_element(v->begin(), v->begin() + i, v->end());
    return at(*v, i);
}

} // namespace detail

constexpr std::tuple<const char*, const char*, const char*> rusage_names = {"utime", "stime", "maxrss"};

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF>
inline std::tuple<long, long, long>
get_rusage() {
#ifdef _WIN32
    return std::make_tuple(0L, 0L, 0L);
#else
    rusage ru;
    getrusage(Who, &ru);
    return std::make_tuple(
        detail::unit<TimePrefix>(ru.ru_utime),
        detail::unit<TimePrefix>(ru.ru_stime),
        detail::unit<MemoryPrefix>(ru.ru_maxrss)
    );
#endif
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF, class Fn>
inline std::tuple<long, long, long>
delta_rusage(Fn&& fun) {
    const auto start = get_rusage<TimePrefix, MemoryPrefix, Who>();
    fun();
    const auto end = get_rusage<TimePrefix, MemoryPrefix, Who>();
    return detail::minus(end, start);
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF, class Fn>
inline std::tuple<long, long, long>
delta_rusage(Fn&& fun, int times) {
    std::vector<long> utime;
    std::vector<long> stime;
    std::vector<long> maxrss;
    reserve(utime, times);
    reserve(stime, times);
    reserve(maxrss, times);
    for (int i = 0; i < times; ++i) {
        const auto ru = delta_rusage<TimePrefix, MemoryPrefix, Who>(std::forward<Fn>(fun));
        utime.push_back(std::get<0>(ru));
        stime.push_back(std::get<1>(ru));
        maxrss.push_back(std::get<2>(ru));
    }
    return std::make_tuple(
        detail::median(&utime),
        detail::median(&stime),
        detail::median(&maxrss)
    );
}

#ifndef _WIN32

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
    auto tuple() const {
        return std::make_tuple(utime, stime, maxrss);
    }
    constexpr static const char* header = "utime\tstime\tmaxrss";
    long utime;
    long stime;
    long maxrss;
};

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF>
[[deprecated("use get_rusage() instead")]]
inline ResourceUsage<TimePrefix, MemoryPrefix, Who>
getrusage() {
    return ResourceUsage<TimePrefix, MemoryPrefix, Who>();
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF, class Fn>
[[deprecated("use delta_rusage() instead")]]
inline ResourceUsage<TimePrefix, MemoryPrefix, Who>
diff_rusage(Fn&& fun) {
    const auto start = ResourceUsage<TimePrefix, MemoryPrefix, Who>();
    fun();
    return ResourceUsage<TimePrefix, MemoryPrefix, Who>(start);
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF, class Fn>
[[deprecated("use delta_rusage() instead")]]
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

[[deprecated("use rusage_names")]]
constexpr const char* rusage_header() {return ResourceUsage<>::header;}

#endif // _WIN32

} // namespace wtl

#endif // WTL_RESOURCE_HPP_

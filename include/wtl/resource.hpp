#pragma once
#ifndef WTL_RESOURCE_HPP_
#define WTL_RESOURCE_HPP_

#include <sys/time.h>
#include <sys/resource.h>
#include <algorithm>
#include <ratio>
#include <vector>
#include <iosfwd>

namespace wtl {

namespace {

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
    const auto i = v->size() / 2u;
    std::nth_element(v->begin(), v->begin() + i, v->end());
    return v->at(i);
}

} // namaspace

inline rusage getrusage(int who = RUSAGE_SELF) {
    rusage ru;
    getrusage(who, &ru);
    return ru;
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, int Who=RUSAGE_SELF>
struct ResourceUsage {
    ResourceUsage(const rusage& ru = getrusage(Who)) noexcept
    : utime(unit<TimePrefix>(ru.ru_utime)),
      stime(unit<TimePrefix>(ru.ru_stime)),
      maxrss(unit<MemoryPrefix>(ru.ru_maxrss)) {}
    ResourceUsage(const long u, const long s, const long m) noexcept
    : utime(u), stime(s), maxrss(m) {}
    ResourceUsage(const ResourceUsage& other) noexcept {
        const rusage ru = getrusage(Who);
        utime = unit<TimePrefix>(ru.ru_utime) - other.utime;
        stime = unit<TimePrefix>(ru.ru_stime) - other.stime;
        maxrss = unit<MemoryPrefix>(ru.ru_maxrss) - other.maxrss;
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
diff_rusage(Fn&& fun, unsigned times) {
    std::vector<long> utime;
    std::vector<long> stime;
    std::vector<long> maxrss;
    utime.reserve(times);
    stime.reserve(times);
    maxrss.reserve(times);
    for (unsigned i = 0u; i < times; ++i) {
        const auto ru = diff_rusage<TimePrefix, MemoryPrefix, Who>(std::forward<Fn>(fun));
        utime.push_back(ru.utime);
        stime.push_back(ru.stime);
        maxrss.push_back(ru.maxrss);
    }
    return ResourceUsage<TimePrefix, MemoryPrefix, Who>(
        median(&utime), median(&stime), median(&maxrss)
    );
}

template <class TimePrefix, class MemoryPrefix, int Who=RUSAGE_SELF>
std::ostream& operator<<(std::ostream& ost, const ResourceUsage<TimePrefix, MemoryPrefix, Who>& x) {
    return x.write(ost);
}

constexpr const char* rusage_header() {return ResourceUsage<>::header;}

} // namespace wtl

#endif // WTL_RESOURCE_HPP_

#pragma once
#ifndef WTL_RESOURCE_HPP_
#define WTL_RESOURCE_HPP_

#include <sys/time.h>
#include <sys/resource.h>
#include <ratio>
#include <string>
#include <iosfwd>

#include "dataframe.hpp"

namespace wtl {

inline timeval operator-(const timeval& lhs, const timeval& rhs) {
    timeval diff = lhs;
    diff.tv_sec  -= rhs.tv_sec;
    diff.tv_usec -= rhs.tv_usec;
    return diff;
}

template <class TimePrefix=std::micro>
class duration {
  public:
    duration(const timeval& tv): tv_(tv) {}
    duration(const timeval& lhs, const timeval& rhs)
    : tv_(lhs - rhs) {}
    long count() const noexcept {
        using FromMicro = std::ratio_divide<TimePrefix, std::micro>;
        return TimePrefix::den * tv_.tv_sec / TimePrefix::num
               + FromMicro::den * tv_.tv_usec / FromMicro::num;
    }
  private:
    timeval tv_;
};

template <class TimePrefix=std::micro>
inline long utime(const rusage& lhs, const rusage& rhs) {
    return duration<TimePrefix>(lhs.ru_utime, rhs.ru_utime).count();
}

template <class TimePrefix=std::micro>
inline long stime(const rusage& lhs, const rusage& rhs) {
    return duration<TimePrefix>(lhs.ru_stime, rhs.ru_stime).count();
}

namespace detail {

template <bool Condition>
using enable_if_t = typename std::enable_if<Condition, std::nullptr_t>::type;

template <class T, detail::enable_if_t<std::is_same<T, std::kilo>{}> = nullptr>
inline long unit(long x) noexcept {
#ifdef __APPLE__
    return x >> 10;
#else
    return x;
#endif
}

template <class T, detail::enable_if_t<std::is_same<T, std::mega>{}> = nullptr>
inline long unit(long x) noexcept {return unit<std::kilo>(x) >> 10;}

template <class T, detail::enable_if_t<std::is_same<T, std::giga>{}> = nullptr>
inline long unit(long x) noexcept {return unit<std::kilo>(x) >> 20;}

}

inline rusage getrusage(int who = RUSAGE_SELF) {
    rusage ru;
    getrusage(who, &ru);
    return ru;
}

inline rusage& ru_epoch(int who = RUSAGE_SELF) {
    static rusage epoch = getrusage(who);
    return epoch;
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo>
struct ResourceUsage {
    ResourceUsage(const rusage& ru_start = ru_epoch(), rusage ru_now = getrusage(RUSAGE_SELF))
    : utime(wtl::utime<TimePrefix>(ru_now, ru_start)),
      stime(wtl::stime<TimePrefix>(ru_now, ru_start)),
      maxrss(detail::unit<MemoryPrefix>(ru_now.ru_maxrss)) {}
    const long utime;
    const long stime;
    const long maxrss;
};

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo>
inline ResourceUsage<TimePrefix, MemoryPrefix>
getrusage(const rusage& ru_start = ru_epoch(), int who = RUSAGE_SELF) {
    return ResourceUsage<TimePrefix, MemoryPrefix>(ru_start, getrusage(who));
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, class Function>
inline ResourceUsage<TimePrefix, MemoryPrefix>
getrusage(Function&& fun) {
    const auto epoch = getrusage();
    fun();
    return getrusage<TimePrefix, MemoryPrefix>(epoch);
}

template <class TimePrefix=std::micro, class MemoryPrefix=std::kilo, class Function>
inline DataFrame
benchmark(Function&& fun, const std::string& label="", unsigned times = 1u) {
    DataFrame df;
    df.reserve_cols(4u)
      .init_column<long>("utime")
      .init_column<long>("stime")
      .init_column<long>("maxrss")
      .init_column<std::string>("label")
      .reserve_rows(times);
    for (unsigned i = 0u; i < times; ++i) {
        auto ru = getrusage<TimePrefix, MemoryPrefix>(std::forward<Function>(fun));
        df.add_row(ru.utime, ru.stime, ru.maxrss, label);
    }
    return df;
}

template <class TimePrefix, class MemoryPrefix>
std::ostream& operator<<(std::ostream& ost, const ResourceUsage<TimePrefix, MemoryPrefix>& x) {
    return ost << x.utime << "\t"
               << x.stime << "\t"
               << x.maxrss;
}

constexpr const char* rusage_header() {return "utime\tstime\tmaxrss";}

} // namespace wtl

#endif // WTL_RESOURCE_HPP_

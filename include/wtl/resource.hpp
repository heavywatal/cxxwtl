#pragma once
#ifndef WTL_RESOURCE_HPP_
#define WTL_RESOURCE_HPP_

#include <sys/time.h>
#include <sys/resource.h>
#include <ratio>

namespace wtl {

inline timeval operator-(const timeval& lhs, const timeval& rhs) {
    timeval diff = lhs;
    diff.tv_sec  -= rhs.tv_sec;
    diff.tv_usec -= rhs.tv_usec;
    return diff;
}

template <class Period=std::micro>
class duration {
  public:
    duration(const timeval& tv): tv_(tv) {}
    duration(const timeval& lhs, const timeval& rhs)
    : tv_(lhs - rhs) {}
    long count() const noexcept {
        constexpr auto denom = std::ratio_multiply<std::mega, Period>::num;
        return Period::den * tv_.tv_sec + tv_.tv_usec / denom;
    }
  private:
    timeval tv_;
};

template <class Period=std::micro>
inline long utime(const rusage& lhs, const rusage& rhs) {
    return duration<Period>(lhs.ru_utime, rhs.ru_utime).count();
}

template <class Period=std::micro>
inline long stime(const rusage& lhs, const rusage& rhs) {
    return duration<Period>(lhs.ru_stime, rhs.ru_stime).count();
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

template <class Period=std::micro, class Memory=std::kilo>
struct ResourceUsage {
    ResourceUsage(const rusage& ru_start = ru_epoch(), int who = RUSAGE_SELF)
    : data(getrusage(who)),
      utime(wtl::utime<Period>(data, ru_start)),
      stime(wtl::stime<Period>(data, ru_start)),
      maxrss(detail::unit<Memory>(data.ru_maxrss)) {}
    const rusage data;
    const long utime;
    const long stime;
    const long maxrss;
};

template <class Period=std::micro, class Memory=std::ratio<1>>
inline ResourceUsage<Period, Memory>
getrusage(const rusage& start = ru_epoch(), int who = RUSAGE_SELF) {
    return ResourceUsage<Period, Memory>(start, who);
}

} // namespace wtl

#endif // WTL_RESOURCE_HPP_

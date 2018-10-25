#pragma once
#ifndef WTL_RESOURCE_HPP_
#define WTL_RESOURCE_HPP_

#include <sys/time.h>
#include <sys/resource.h>
#include <chrono>

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

} // namespace wtl

#endif /* WTL_RESOURCE_HPP_ */

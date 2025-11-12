#pragma once
#ifndef WTL_CHRONO_HPP_
#define WTL_CHRONO_HPP_

#include <ctime>

#include <chrono>
#include <string>
#include <sstream>
#include <iostream>

namespace wtl {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// speed test

#if __cplusplus < 202002L

template <class Rep, class Period> inline
std::ostream& operator<<(std::ostream& ost, const std::chrono::duration<Rep, Period>& d) {
    ost << d.count();
    if constexpr (std::is_same_v<Period, std::nano>) {
      ost << "ns";
    } else if constexpr (std::is_same_v<Period, std::micro>) {
      ost << "us";
    } else if constexpr (std::is_same_v<Period, std::milli>) {
      ost << "ms";
    } else if constexpr (std::is_same_v<Period, std::ratio<1>>) {
      ost << "s";
    }
    return ost;
}

#endif

template <class Duration = std::chrono::milliseconds, class Fn> inline
Duration stopwatch(Fn&& fn) {
    using Clock = std::chrono::high_resolution_clock;
    const auto start = Clock::now();
    fn();
    return std::chrono::duration_cast<Duration>(Clock::now() - start);
}

template <class Duration = std::chrono::milliseconds, class Fn> inline
std::ostream& stopwatch(std::ostream& ost, Fn&& fn, int times=3) {
    for (; times > 0; --times) {
        ost << stopwatch<Duration>(std::forward<Fn>(fn)) << "\t" << std::flush;
    }
    return ost << "\n";
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// datetime

// default is the same as ctime(): Thu Aug 23 14:55:02 2001
// which is equivalent to "%a %b %d %T %Y"
inline std::string strftime(const char* format="%c") {
    char cstr[256];
    const std::time_t raw_time = std::time(nullptr);
    const struct std::tm* t = std::localtime(&raw_time);
    std::strftime(cstr, sizeof(cstr), format, t);
    return std::string(cstr);
}
inline std::string iso8601date(std::string_view sep="-") {
    std::ostringstream oss;
    oss << "%Y" << sep << "%m" << sep << "%d";
    return strftime(oss.str().c_str());
}
inline std::string iso8601time(std::string_view sep=":") {
    std::ostringstream oss;
    oss << "%H" << sep << "%M" << sep << "%S";
    return strftime(oss.str().c_str());
}
inline std::string iso8601datetime() {return strftime("%FT%T%z");}

} // namespace wtl

#endif // WTL_CHRONO_HPP_

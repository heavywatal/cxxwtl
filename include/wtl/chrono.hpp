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

template <typename Time = std::chrono::milliseconds, typename Fn>
inline Time stopwatch (Fn&& fn) {
    using Clock = std::chrono::high_resolution_clock;
    const auto start = Clock::now();
    fn();
    return std::chrono::duration_cast<Time>(Clock::now() - start);
}

template <class Fn>
void benchmark(Fn&& fn, std::string_view label="", size_t times=3) {
    std::cerr << "#BENCHMARK[ms] " << label << "\n";
    for (; times>0; --times) {
        std::cerr << stopwatch(fn).count() << "\t" << std::flush;
    }
    std::cout << "\n";
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// datetime

// default is the same as ctime(): Thu Aug 23 14:55:02 2001
// which is equivalent to "%a %b %d %T %Y"
inline std::string strftime(const std::string& format="%c") {
    char cstr[80];
    const time_t raw_time = time(nullptr);
    const struct tm* t = localtime(&raw_time);
    std::strftime(cstr, sizeof(cstr), format.c_str(), t);
    return std::string(cstr);
}
inline std::string iso8601date(std::string_view sep="-") {
    std::ostringstream oss;
    oss << "%Y" << sep << "%m" << sep << "%d";
    return strftime(oss.str());
}
inline std::string iso8601time(std::string_view sep=":") {
    std::ostringstream oss;
    oss << "%H" << sep << "%M" << sep << "%S";
    return strftime(oss.str());
}
inline std::string iso8601datetime() {return strftime("%FT%T%z");}

} // namespace wtl

#endif // WTL_CHRONO_HPP_

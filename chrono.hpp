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
    typedef std::chrono::high_resolution_clock Clock;
    const auto start = Clock::now();
    fn();
    return std::chrono::duration_cast<Time>(Clock::now() - start);
}

template <class Fn>
void benchmark(Fn&& fn, const std::string& label="", size_t times=3) {
    for (; times>0; --times) {
        const auto t = stopwatch(fn);
        std::cerr << "#BENCHMARK "
                  << t.count() << " [ms] " << label << std::endl;
    }
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// datetime

// default is the same as ctime(): Thu Aug 23 14:55:02 2001
// which is equivalent to "%a %b %d %T %Y"
inline std::string strftime(const std::string& format="%c") {
    char cstr[80];
    const time_t rawtime = time(nullptr);
    const struct tm* t = localtime(&rawtime);
    std::strftime(cstr, sizeof(cstr), format.c_str(), t);
    return std::string(cstr);
}
inline std::string iso8601date(const std::string& sep="-") {
    std::ostringstream oss;
    oss << "%Y" << sep << "%m" << sep << "%d";
    return strftime(oss.str());
}
inline std::string iso8601time(const std::string& sep=":") {
    std::ostringstream oss;
    oss << "%H" << sep << "%M" << sep << "%S";
    return strftime(oss.str());
}
inline std::string iso8601datetime() {return strftime("%FT%T%z");}

} // namespace wtl

#endif /* WTL_CHRONO_HPP_ */

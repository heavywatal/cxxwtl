// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_EXCEPTION_HPP_
#define WTL_EXCEPTION_HPP_

#include <csignal>
#include <stdexcept>
#include <atomic>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class ExitSuccess: public std::runtime_error {
  public:
    ExitSuccess(const char* msg="EXIT_SUCCESS"): std::runtime_error(msg) {}
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Translate POSIX/C signal to exception
// {
//     set_SIGINT_handler();
//
//     if (wtl::SIGINT_RAISED()) {throw wtl::KeyboardInterrupt();}
// }

class KeyboardInterrupt: public std::runtime_error {
  public:
    KeyboardInterrupt(const char* msg="KeyboardInterrupt"): std::runtime_error(msg) {}
};

inline std::atomic_bool& SIGINT_RAISED() {
    static std::atomic_bool is_raised(false);
    return is_raised;
}

inline void set_SIGINT_handler() {
    std::signal(SIGINT, [](int signum) {
        signum += 0; // suppress warning of unused parameter
        SIGINT_RAISED() = true;
    });
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_EXCEPTION_HPP_

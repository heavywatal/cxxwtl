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
    ExitSuccess(const char* msg="EXIT_SUCCESS") noexcept:
      std::runtime_error(msg) {}
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
    KeyboardInterrupt(const char* msg="KeyboardInterrupt") noexcept:
      std::runtime_error(msg) {}
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

class AssertionError: public std::runtime_error {
  public:
    AssertionError(const char* message) noexcept:
      std::runtime_error(message) {}
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#define WTL_STR(x) #x
#define WTL_STR_PP(x) WTL_STR(x)

#define WTL_ASSERT(EXPR) if (!(EXPR)) {\
    throw wtl::AssertionError("\n" __FILE__ ":" WTL_STR_PP(__LINE__)\
                              ": AssertionError\n  " #EXPR);\
}

#endif // WTL_EXCEPTION_HPP_

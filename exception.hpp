// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include <exception>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class ExitSuccess: public std::exception {
  public:
    ExitSuccess() noexcept: std::exception() {}
    const char* what() const noexcept {return "EXIT_SUCCESS";}
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // EXCEPTION_HPP_

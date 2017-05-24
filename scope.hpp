// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_SCOPE_HPP_
#define WTL_SCOPE_HPP_

#include <utility>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class Function>
class ScopeExit {
  public:
    ScopeExit(Function&& f): func_(std::forward<Function>(f)) {}
    ~ScopeExit() {func_();}
    ScopeExit(const ScopeExit&) = delete;
    ScopeExit& operator=(const ScopeExit&) = delete;
    ScopeExit(ScopeExit&&) = default;
    ScopeExit& operator=(ScopeExit&&) = default;
  private:
    Function func_;
};

template <class Function>
ScopeExit<Function> scope_exit(Function&& func) {
    return ScopeExit<Function>(std::forward<Function>(func));
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_SCOPE_HPP_ */

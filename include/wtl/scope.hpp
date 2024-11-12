#pragma once
#ifndef WTL_SCOPE_HPP_
#define WTL_SCOPE_HPP_

#include <utility>

namespace wtl {

template <class Function>
class ScopeExit {
  private:
    Function func_;
  public:
    ScopeExit(Function&& f) noexcept: func_(std::forward<Function>(f)) {}
    ~ScopeExit() noexcept(noexcept(func_())) {func_();}
    ScopeExit(const ScopeExit&) = delete;
    ScopeExit& operator=(const ScopeExit&) = delete;
    ScopeExit(ScopeExit&&) noexcept = default;
    ScopeExit& operator=(ScopeExit&&) noexcept = default;
};

template <class Function> inline
ScopeExit<Function> scope_exit(Function&& func) {
    return ScopeExit<Function>(std::forward<Function>(func));
}

} // namespace wtl

#endif // WTL_SCOPE_HPP_

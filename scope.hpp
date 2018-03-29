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
    ScopeExit(Function&& f) noexcept: func_(std::move(f)) {}
    ~ScopeExit() noexcept(noexcept(func_())) {func_();}
    ScopeExit(const ScopeExit&) = delete;
    ScopeExit& operator=(const ScopeExit&) = delete;
    ScopeExit(ScopeExit&&) noexcept = default;
    ScopeExit& operator=(ScopeExit&&) noexcept = default;
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

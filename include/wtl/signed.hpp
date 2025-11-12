#pragma once
#ifndef WTL_SIGNED_HPP_
#define WTL_SIGNED_HPP_

#include <type_traits>
#include <cstddef>

namespace wtl {

using idx_t = std::ptrdiff_t;

template <class Container> inline
constexpr auto ssize(const Container& cont) noexcept {
  return static_cast<std::make_signed_t<decltype(cont.size())>>(cont.size());
}

template <class T> inline
constexpr std::make_unsigned_t<T> cast_u(T x) noexcept {
    return static_cast<std::make_unsigned_t<T>>(x);
}

template <class Container, class IntType> inline
constexpr auto& at(Container& cont, const IntType i) {
    return cont.at(cast_u(i));
}

template <class Container, class IntType> inline
void resize(Container& cont, const IntType n) {
    cont.resize(cast_u(n));
}

template <class Container, class IntType> inline
void reserve(Container& cont, const IntType n) {
    cont.reserve(cast_u(n));
}

} // namespace wtl

#endif // WTL_SIGNED_HPP_

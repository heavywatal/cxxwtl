#pragma once
#ifndef WTL_STRING_HPP_
#define WTL_STRING_HPP_

#include <string>
#include <charconv>
#include <vector>

namespace wtl {

template <class T> inline T
sto(std::string_view sv) {
  T x{};
  std::from_chars(std::begin(sv), std::end(sv), x);
  return x;
}

template <> inline std::string_view
sto(std::string_view sv) {return sv;}

template <> inline std::string
sto(std::string_view sv) {return std::string{sv};}

template <> inline bool
sto(std::string_view sv) {return static_cast<bool>(sto<int>(sv));}

template <> inline double
sto(std::string_view sv) {return std::stod(std::string{sv});}

template <class T> inline void
split(std::string_view src, std::string_view delimiter, T* dst) {
    if (src.empty()) return;
    using npos_t = decltype(src.npos);
    for (auto start = npos_t{}, pos = npos_t{}; pos != src.npos; start = pos + 1u) {
        pos = src.find_first_of(delimiter, start);
        dst->push_back(sto<typename T::value_type>(src.substr(start, pos - start)));
    }
}

template <class T = std::string> inline std::vector<T>
split(std::string_view src, std::string_view delimiter=" \t\n") {
    std::vector<T> dst;
    split(src, delimiter, &dst);
    return dst;
}

inline std::string_view rstrip(std::string_view s, std::string_view chars=" ") {
    return s.substr(0u, s.find_last_not_of(chars) + 1u);
}

inline std::string_view lstrip(std::string_view s, std::string_view chars=" ") {
    return s.substr(s.find_first_not_of(chars));
}

inline std::string_view strip(std::string_view s, std::string_view chars=" ") {
    return rstrip(lstrip(s, chars), chars);
}

inline bool starts_with(std::string_view str, std::string_view prefix) {
    return (str.size() >= prefix.size()) &&
           (str.compare(0, prefix.size(), prefix) == 0);
}

inline bool ends_with(std::string_view str, std::string_view suffix) {
    auto str_size = str.size();
    return (str_size >= suffix.size()) &&
           (str.compare(str_size -= suffix.size(), suffix.size(), suffix) == 0);
}

// Use std::replace() in <algorithm> for single char substitution
inline void replace(std::string* s, std::string_view patt, std::string_view repl) {
    const auto pattsize = patt.size();
    std::string::size_type pos = 0u;
    while ((pos = s->find(patt, pos)) != std::string::npos) {
        s->replace(pos, pattsize, repl);
        pos += pattsize;
    }
}

inline std::string replace(std::string s, std::string_view patt, std::string_view repl) {
    replace(&s, patt, repl);
    return s;
}

} // namespace wtl

#endif // WTL_STRING_HPP_

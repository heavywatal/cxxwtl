#pragma once
#ifndef WTL_STRING_HPP_
#define WTL_STRING_HPP_

#include <string>
#include <vector>

namespace wtl {

template <class T> inline T
sto(const std::string& s) {return s;}

template <> inline const char*
sto(const std::string& s) {return s.c_str();}

template <> inline bool
sto<bool>(const std::string& s) {return static_cast<bool>(std::stoi(s));}

template <> inline int
sto<int>(const std::string& s) {return std::stoi(s);}

template <> inline long
sto<long>(const std::string& s) {return std::stol(s);}

template <> inline long long
sto<long long>(const std::string& s) {return std::stoll(s);}

template <> inline unsigned
sto<unsigned>(const std::string& s) {return static_cast<unsigned>(std::stoul(s));}

template <> inline unsigned long
sto<unsigned long>(const std::string& s) {return std::stoul(s);}

template <> inline unsigned long long
sto<unsigned long long>(const std::string& s) {return std::stoull(s);}

template <> inline double
sto<double>(const std::string& s) {return std::stod(s);}

template <class T> inline void
split(const std::string& src, const std::string& delimiter, T* dst) {
    if (src.empty()) return;
    for (size_t start = 0, pos = 0; pos != src.npos; start = pos + 1u) {
        pos = src.find_first_of(delimiter, start);
        dst->push_back(sto<typename T::value_type>(src.substr(start, pos - start)));
    }
}

template <class T = std::string> inline std::vector<T>
split(const std::string& src, const std::string& delimiter=" \t\n") {
    std::vector<T> dst;
    split(src, delimiter, &dst);
    return dst;
}

inline std::string rstrip(const std::string& s, const std::string& chars=" ") {
    return s.substr(0u, s.find_last_not_of(chars) + 1u);
}

inline std::string lstrip(const std::string& s, const std::string& chars=" ") {
    return s.substr(s.find_first_not_of(chars));
}

inline std::string strip(const std::string& s, const std::string& chars=" ") {
    return rstrip(lstrip(s, chars), chars);
}

inline bool startswith(const std::string& str, const std::string& prefix) {
    return (str.size() >= prefix.size()) &&
           (str.compare(0, prefix.size(), prefix) == 0);
}

inline bool endswith(const std::string& str, const std::string& suffix) {
    size_t str_size = str.size();
    return (str_size >= suffix.size()) &&
           (str.compare(str_size -= suffix.size(), suffix.size(), suffix) == 0);
}

// Use std::replace() in <algorithm> for single char substitution
inline void replace(std::string* s, const std::string& patt, const std::string& repl) {
    const auto pattsize = patt.size();
    std::string::size_type pos = 0u;
    while ((pos = s->find(patt, pos)) != std::string::npos) {
        s->replace(pos, pattsize, repl);
        pos += pattsize;
    }
}

inline std::string replace(std::string s, const std::string& patt, const std::string& repl) {
    replace(&s, patt, repl);
    return s;
}

} // namespace wtl

#endif // WTL_STRING_HPP_

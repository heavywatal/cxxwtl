#pragma once
#ifndef WTL_STRING_HPP_
#define WTL_STRING_HPP_

#include <string>
#include <vector>

namespace wtl {

inline std::vector<std::string>
split(const std::string& src, const std::string& delimiter=" \t\n") {
    if (src.empty()) return {};
    std::vector<std::string> dst;
    for (size_t start = 0, pos = 0; pos != src.npos; start = pos + 1u) {
        pos = src.find_first_of(delimiter, start);
        dst.push_back(src.substr(start, pos - start));
    }
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

inline std::string replace_all(const std::string& patt, const std::string& repl, const std::string& src) {
    std::string result;
    std::string::size_type pos_before(0);
    std::string::size_type pos(0);
    std::string::size_type len(patt.size());
    while ((pos = src.find(patt, pos)) != std::string::npos) {
        result.append(src, pos_before, pos-pos_before);
        result.append(repl);
        pos += len;
        pos_before = pos;
    }
    result.append(src, pos_before, src.size()-pos_before);
    return result;
}

} // namespace wtl

#endif // WTL_STRING_HPP_

#pragma once
#ifndef WTL_CLIPP_HPP_
#define WTL_CLIPP_HPP_

#include <clipp.h>
#include <nlohmann/json.hpp>

#include <type_traits>
#include <vector>
#include <string>

namespace wtl {

namespace detail {

struct nonempty {
    bool operator()(const std::string& s) {return !s.empty();}
};

template <bool Condition>
using enable_if_t = typename std::enable_if<Condition, std::nullptr_t>::type;

template <class T, enable_if_t<std::is_integral<T>{}> = nullptr>
inline auto filter_type(T) {
    return clipp::match::integers{};
}

template <class T, enable_if_t<std::is_floating_point<T>{}> = nullptr>
inline auto filter_type(T) {
    return clipp::match::numbers{};
}

template <class T, enable_if_t<!std::is_trivial<T>{}> = nullptr>
inline auto filter_type(T) {
    return nonempty{};
}

template <class T, enable_if_t<std::is_pointer<T>{}> = nullptr>
inline auto filter_type(T) {
    return nonempty{};
}

template <class T> inline
std::string doc_default(const T& x, const std::string& doc) {
    std::ostringstream oss;
    oss << doc << " (=" << x << ")";
    return oss.str();
}

template <typename T> inline
auto set(nlohmann::json& obj, const char* key) {
    return [&j = obj[key]](const char* s){j = s;};
}

template <> inline
auto set<bool>(nlohmann::json& obj, const char* key) {
    return [&j = obj[key]](){j = true;};
}

template <> inline
auto set<int>(nlohmann::json& obj, const char* key) {
    return [&j = obj[key]](const char* s){j = std::stoi(s);};
}

template <> inline
auto set<long>(nlohmann::json& obj, const char* key) {
    return [&j = obj[key]](const char* s){j = std::stol(s);};
}

template <> inline
auto set<unsigned>(nlohmann::json& obj, const char* key) {
    return [&j = obj[key]](const char* s){j = static_cast<unsigned>(std::stoul(s));};
}

template <> inline
auto set<unsigned long>(nlohmann::json& obj, const char* key) {
    return [&j = obj[key]](const char* s){j = std::stoul(s);};
}

template <> inline
auto set<unsigned long long>(nlohmann::json& obj, const char* key) {
    return [&j = obj[key]](const char* s){j = std::stoull(s);};
}

template <> inline
auto set<double>(nlohmann::json& obj, const char* key) {
    return [&j = obj[key]](const char* s){j = std::stod(s);};
}

} // namespace detail

template <class T> inline clipp::group
option(std::vector<std::string>&& flags, T* target, const std::string& doc="", const std::string& label="arg") {
    return (
      clipp::option(std::move(flags)) &
      clipp::value(detail::filter_type(*target), label, *target)
    ) % detail::doc_default(*target, doc);
}

inline clipp::parameter
option_bool(std::vector<std::string>&& flags, bool* target, const std::string& doc="") {
    return clipp::option(std::move(flags)).set(*target).doc(doc);
}

template <class T> inline clipp::group
option(nlohmann::json& obj, std::vector<std::string>&& flags, const T init, const std::string& doc="", const std::string& label="arg") {
    const char* key = flags.back().c_str();
    obj[key] = init;
    return (
      clipp::option(std::move(flags)) &
      clipp::value(detail::filter_type(init), label).call(detail::set<T>(obj, key))
    ) % detail::doc_default(obj[key], doc);
}

template <class T> inline clipp::group
option(nlohmann::json& obj, std::vector<std::string>&& flags, T* target, const std::string& doc="", const std::string& label="arg") {
    auto group = option(obj, std::move(flags), *target, doc, label);
    auto& value = group.back().as_param();
    value.call(clipp::set(*target));
    return group;
}

inline clipp::parameter
option_bool(nlohmann::json& obj, std::vector<std::string>&& flags, const std::string& doc="") {
    const char* key = flags.back().c_str();
    obj[key] = false;
    return clipp::option(std::move(flags)).call(detail::set<bool>(obj, key)).doc(doc);
}

inline clipp::doc_formatting doc_format() {
    return clipp::doc_formatting{}
      .first_column(0)
      .doc_column(24)
      .last_column(80)
      .indent_size(2)
      .flag_separator(",")
    ;
}

inline std::ostream& usage(
  std::ostream& ost,
  const clipp::group& cli,
  const std::string& program="PROGRAM",
  clipp::doc_formatting fmt=doc_format()) {
    return ost << clipp::usage_lines(cli, program, fmt) << "\n\n"
               << clipp::documentation(cli, fmt) << "\n";
}

}  // namespace wtl

#endif /* WTL_CLIPP_HPP_ */

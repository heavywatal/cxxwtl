#pragma once
#ifndef WTL_IOSTR_HPP_
#define WTL_IOSTR_HPP_

#include "string.hpp"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>

#include <vector>
#include <valarray>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

namespace wtl {

class null_streambuf: public std::streambuf {
  public:
    int overflow(int c) {return c;}
};

class null_ostream: public std::ostream {
  public:
    null_ostream(): std::ostream(&streambuf_) {}
  private:
    null_streambuf streambuf_;
};

inline null_streambuf* nstreambuf() {
    static null_streambuf streambuf;
    return &streambuf;
}

inline null_ostream& nout() {
    static null_ostream ostream;
    return ostream;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// stream factory

namespace fs = std::filesystem;

[[deprecated("use std::ofstream directly")]]
inline std::ofstream
make_ofs(const fs::path& filepath, std::ios_base::openmode mode=std::ios_base::out) {
    std::ofstream ofs(filepath, mode | std::ios_base::binary);
    ofs.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    ofs.precision(std::cout.precision());
    return ofs;
}

[[deprecated("use std::ifstream directly")]]
inline std::ifstream
make_ifs(const fs::path& filepath, std::ios_base::openmode mode=std::ios_base::in) {
    std::ifstream ifs(filepath, mode | std::ios_base::binary);
    ifs.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    return ifs;
}

inline std::ostringstream
make_oss(const std::streamsize precision=std::cout.precision(),
         const std::ios_base::fmtflags fmtfl=std::cout.flags()) {
    std::ostringstream oss;
    oss.precision(precision);
    oss.setf(fmtfl);
    return oss;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// stream manipulator

namespace detail {
  template <typename CharT>
  class SetFillW {
    public:
      SetFillW(CharT ch, std::streamsize width) noexcept:
        ch_(ch), width_(width) {}
      friend std::ostream& operator<<(std::ostream& ost, const SetFillW<CharT>& x) noexcept {
          ost.fill(x.ch_);
          ost.width(x.width_);
          return ost;
      }
    private:
      const CharT ch_;
      const std::streamsize width_;
  };
} // namespace detail

template <typename CharT>
inline detail::SetFillW<CharT> setfillw(CharT fill, std::streamsize width) {
    return detail::SetFillW<CharT>(fill, width);
}

[[deprecated("use wtl::setfillw")]]
inline detail::SetFillW<char> setfill0w(std::streamsize width) {
    return detail::SetFillW<char>('0', width);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Reader

template <class T> inline
void read(std::istream& ist, std::vector<T>* v) {
    v->assign(std::istream_iterator<T>(ist), std::istream_iterator<T>{});
}

template <class T> inline
void read(const std::string& line, std::vector<T>* v) {
    std::istringstream iss(line);
    read(iss, v);
}

template <size_t I=0u, typename... Ts> inline
void read(std::istream& ist, std::tuple<Ts...>* x) {
    if constexpr (I < sizeof...(Ts)) {
        ist >> std::get<I>(*x);
        read<I + 1u>(ist, x);
    }
}

template <typename... Ts> inline
void read(const std::string& line, std::tuple<Ts...>* x) {
    std::istringstream iss(line);
    read<0u>(iss, x);
}

template <class T> inline
std::istream& getline(std::istream& ist, T& buffer) {
    std::string buffer_impl;
    auto& ret = std::getline(ist, buffer_impl);
    if (ret) read(buffer_impl, &buffer);
    return ret;
}

template <> inline
std::istream& getline<std::string>(std::istream& ist, std::string& buffer) {
    return std::getline(ist, buffer);
}

template <class T = std::string> inline
std::vector<T> readlines(std::istream& ist) {
    std::vector<T> lines;
    T buffer;
    while (wtl::getline(ist, buffer)) {
        lines.push_back(buffer);
    }
    return lines;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Writer

inline std::ostream& write_if_avail(std::ostream& ost, std::streambuf* buf) {
    if (buf->in_avail()) ost << buf;
    return ost;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// experimental iterator

struct Forward {
    template<class T>
    constexpr T operator()(T&& x) const noexcept {
        return std::forward<T>(x);
    }
};

template <class DelimT, class Func=Forward, class charT=char, class traitsT=std::char_traits<charT>>
class ostream_joiner {
  public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using char_type = charT;
    using traits_type = traitsT;
    using ostream_type = std::basic_ostream<char_type, traits_type>;

    ostream_joiner(ostream_type& s, DelimT&& d, Func&& f)
    : ost_(&s), delim_(std::move(d)), func_(std::move(f)) {}
    ostream_joiner(ostream_type& s, const DelimT& d, Func&& f)
    : ost_(&s), delim_(d), func_(std::move(f)) {}

    template <class T>
    ostream_joiner& operator=(T const &item) {
        if (is_not_first_) {
            *ost_ << delim_;
        } else {
            is_not_first_ = true;
        }
        *ost_ << func_(item);
        return *this;
    }
    ostream_joiner &operator*() noexcept {return *this;}
    ostream_joiner &operator++() noexcept {return *this;}
    ostream_joiner &operator++(int) noexcept {return *this;}
  private:
    ostream_type* ost_;
    DelimT delim_;
    Func func_;
    bool is_not_first_ = false;
};

template <class charT, class traits, class DelimT, class Func=Forward>
ostream_joiner<typename std::decay<DelimT>::type, Func, charT, traits>
make_ostream_joiner(std::basic_ostream<charT, traits>& ost, DelimT&& delim, Func&& func=Func{}) {
    return ostream_joiner<typename std::decay<DelimT>::type, Func, charT, traits>(ost, std::forward<DelimT>(delim), std::forward<Func>(func));
}

template <class Iter, class charT, class traits, class DelimT, class Func=Forward>
std::basic_ostream<charT, traits>&
join(Iter begin, const Iter end, std::basic_ostream<charT, traits>& ost, DelimT&& delim, Func&& func=Func{}) {
    std::copy(begin, end, wtl::make_ostream_joiner(ost, std::forward<DelimT>(delim), std::forward<Func>(func)));
    return ost;
}

template <class T, class charT, class traits, class DelimT, class Func=Forward>
std::basic_ostream<charT, traits>&
join(const T& v, std::basic_ostream<charT, traits>& ost, DelimT&& delim, Func&& func=Func{}) {
    std::copy(std::begin(v), std::end(v), wtl::make_ostream_joiner(ost, std::forward<DelimT>(delim), std::forward<Func>(func)));
    return ost;
}

template <class Iter, class DelimT=const char*, class Func=Forward> inline
std::string str_join(Iter begin_, Iter end_, DelimT&& delim=",",
                     std::ostringstream&& oss=make_oss(), Func&& func=Func{}) {
    join(begin_, end_, oss, std::forward<DelimT>(delim), std::forward<Func>(func));
    return oss.str();
}

template <class T, class DelimT=const char*, class Func=Forward> inline
std::string str_join(const T& v, DelimT&& delim=",",
                     std::ostringstream&& oss=make_oss(), Func&& func=Func{}) {
    return str_join(begin(v), end(v), std::forward<DelimT>(delim), std::move(oss), std::forward<Func>(func));
}

template <class T, class DelimT=const char*, class Func=Forward> inline
std::string str_matrix(const T& m, DelimT&& delim=",",
                       std::ostringstream&& oss=make_oss(), Func&& func=Func{}) {
    for (const auto& row: m) {
        join(row, oss, std::forward<DelimT>(delim), std::forward<Func>(func)) << '\n';
    }
    return oss.str();
}

namespace detail {
    template <class Map> inline
    std::ostream& operator_ost_map(std::ostream& ost, const Map& m) {
        ost << '{';
        if (!m.empty()) {
            auto it = std::begin(m);
            ost << std::get<0>(*it) << ": " << std::get<1>(*it);
            while (++it != std::end(m)) {
                ost << ", " << std::get<0>(*it) << ": " << std::get<1>(*it);
            }
        }
        return ost << '}';
    }
} // namespace detail

// template<class Tuple, std::size_t... Is>
// void print_tuple(std::ostream& ost, const Tuple& x, seq<Is...>){
//     ost << (Is == 0 ? "" : ", ") << std::get<Is>(x);
// }

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// global operator<< for containers

namespace std {

template <class T, std::size_t N> inline
std::ostream& operator<< (std::ostream& ost, const std::array<T, N>& v) {
    ost << '[';
    std::copy(std::begin(v), std::end(v), wtl::make_ostream_joiner(ost, ", "));
    return ost << ']';
}

template <class T> inline
std::ostream& operator<< (std::ostream& ost, const std::valarray<T>& v) {
    ost << '[';
    std::copy(std::begin(v), std::end(v), wtl::make_ostream_joiner(ost, ", "));
    return ost << ']';
}

template <class T> inline
std::ostream& operator<< (std::ostream& ost, const std::vector<T>& v) {
    ost << '[';
    std::copy(std::begin(v), std::end(v), wtl::make_ostream_joiner(ost, ", "));
    return ost << ']';
}

template <> inline
std::ostream& operator<< (std::ostream& ost, const std::vector<std::string>& v) {
    if (v.empty()) {return ost << "[]";}
    ost << "['";
    std::copy(std::begin(v), std::end(v), wtl::make_ostream_joiner(ost, "', '"));
    return ost << "']";
}

template <class T> inline
std::ostream& operator<< (std::ostream& ost, const std::set<T>& v) {
    ost << "set([";
    std::copy(v.begin(), v.end(), wtl::make_ostream_joiner(ost, ", "));
    return ost << "])";
}

template <class T, class Pred> inline
std::ostream& operator<< (std::ostream& ost, const std::unordered_set<T, std::hash<T>, Pred>& v) {
    ost << "set([";
    std::copy(v.begin(), v.end(), wtl::make_ostream_joiner(ost, ", "));
    return ost << "])";
}

template <class F, class S> inline
std::ostream& operator<< (std::ostream& ost, const std::pair<F, S>& p) {
    return ost << '['  << std::get<0>(p)
               << ", " << std::get<1>(p) << ']';
}

// map
template <class Key, class T, class Comp> inline
std::ostream& operator<< (std::ostream& ost, const std::map<Key, T, Comp>& m) {
    return wtl::detail::operator_ost_map(ost, m);
}

// multimap
template <class Key, class T, class Comp> inline
std::ostream& operator<< (std::ostream& ost, const std::multimap<Key, T, Comp>& m) {
    return wtl::detail::operator_ost_map(ost, m);
}

// unordered_map
template <class Key, class T, class Hash> inline
std::ostream& operator<< (std::ostream& ost, const std::unordered_map<Key, T, Hash>& m) {
    return wtl::detail::operator_ost_map(ost, m);
}

}  // namespace std

#endif /* WTL_IOSTR_HPP_ */

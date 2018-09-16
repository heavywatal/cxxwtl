#pragma once
#ifndef WTL_IOSTR_HPP_
#define WTL_IOSTR_HPP_

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

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// stream factory

inline std::ofstream
make_ofs(const std::string& filepath, std::ios_base::openmode mode=std::ios_base::out) {
    std::ofstream ofs(filepath, mode | std::ios_base::binary);
    ofs.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    ofs.precision(std::cout.precision());
    return ofs;
}

inline std::ifstream
make_ifs(const std::string& filepath, std::ios_base::openmode mode=std::ios_base::in) {
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

inline detail::SetFillW<char> setfill0w(std::streamsize width) {
    return detail::SetFillW<char>('0', width);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// std::string manipulation

inline std::vector<std::string> split_algorithm(const std::string& src, const std::string& delimiter=" \t\n") {
    std::vector<std::string> dst;
    for (auto delim_pos=begin(src), start=delim_pos; delim_pos!=end(src); ) {
        delim_pos = std::find_first_of(start, end(src), begin(delimiter), end(delimiter));
        dst.push_back(std::string(start, delim_pos));
        ++(start = delim_pos);
    }
    return dst;
}

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

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// Reader

inline std::vector<std::string> readlines(std::istream& ist) {
    std::vector<std::string> lines;
    std::string buffer;
    while (std::getline(ist, buffer)) {
        lines.push_back(buffer);
    }
    return lines;
}

inline std::vector<std::pair<std::string, std::string> >
read_ini(std::istream& ist) {
    std::vector<std::string> lines = readlines(ist);
    std::vector<std::pair<std::string, std::string> > dst;
    dst.reserve(lines.size());
    for (auto line_: lines) {
        line_ = strip(line_);
        if (startswith(line_, "[")) {continue;} // TODO
        auto pair_ = split(line_, ":="); // TODO
        if (pair_.size() < 2) {continue;}
        dst.emplace_back(rstrip(pair_[0]), lstrip(pair_[1]));
    }
    return dst;
}

inline std::vector<std::string>
read_header(std::istream& ist, const char* sep="\t") {
    std::string buffer;
    std::getline(ist, buffer, '\n');
    return split(buffer, sep);
}

template <class T> inline std::vector<std::valarray<T>>
read_valarrays(std::istream& ist) {
    std::vector<std::valarray<T>> matrix;
    std::string buffer;
    std::istream_iterator<T> end;
    while (std::getline(ist, buffer)) {
        std::istringstream iss(buffer);
        std::vector<T> vec(std::istream_iterator<T>(iss), end);
        matrix.emplace_back(vec.data(), vec.size());
    }
    return matrix;
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
class ostream_joiner : public std::iterator<std::output_iterator_tag,void,void,void,void> {
  public:
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

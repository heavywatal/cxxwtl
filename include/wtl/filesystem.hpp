#pragma once
#ifndef WTL_FILESYSTEM_HPP_
#define WTL_FILESYSTEM_HPP_

#ifdef _WIN32
  #include <direct.h>
  #include <io.h>
  #define mkdir(name, mode) _mkdir(name)
  #define chdir _chdir
  #define getcwd _getcwd
  #define access _access
  #define F_OK 0
#else
  #include <sys/stat.h>
  #include <unistd.h>
#endif
#include <cerrno>
#include <stdexcept>
#include <string>
#include <ostream>

namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace filesystem {

class path {
  public:
    using value_type = char;
    using string_type = std::basic_string<value_type>;
#ifdef _WIN32
    static constexpr value_type preferred_separator = '\\';
#else
    static constexpr value_type preferred_separator = '/';
#endif

    path(string_type&& x): data_(std::move(x)) {to_native(data_);}

    template <class T>
    path(const T& x): data_(x) {to_native(data_);}

    path parent_path() const {
        auto pos = data_.find_last_of(preferred_separator);
        if (pos == string_type::npos) return path("");
        if (pos == 0u) return path("/");
        return path(data_.substr(0u, pos));
    }
    path filename() const {
        auto pos = data_.find_last_of(preferred_separator);
        return path(data_.substr(++pos));
    }
    path stem() const {
        auto name = filename().native();
        auto pos = name.find_last_of('.');
        if (pos == string_type::npos || pos == 0u) return path(name);
        if (pos == name.size() - 1u && name == "..") return path(name);
        return path(name.substr(0u, pos));
    }
    path extension() const {
        auto name = filename().native();
        auto pos = name.find_last_of('.');
        if (pos == string_type::npos || pos == 0u) return path("");
        if (pos == name.size() - 1u && name == "..") return path("");
        return path(name.substr(pos));
    }
    path& append(const path& p) {
        if (p.is_absolute()) {
            data_ = p.data_;
        } else {
            if (data_.back() != preferred_separator) {
                data_ += preferred_separator;
            }
            data_ += p.native();
        }
        return *this;
    }
    path& operator/=(const path& p) {
        return append(p);
    }
    path& concat(const path& p) {
        data_ += p.native();
        return *this;
    }
    path& operator+=(const path& p) {
        return concat(p);
    }
    friend path operator/(const path& lhs, const path& rhs) {
        return path(lhs.native()) /= rhs;
    }
    friend bool operator==(const path& lhs, const path& rhs) noexcept {
        return lhs.data_ == rhs.data_;
    }
    friend bool operator!=(const path& lhs, const path& rhs) noexcept {
        return lhs.data_ != rhs.data_;
    }
    friend std::ostream& operator<<(std::ostream& ost, const path& p) {
        return ost << '"' << p.string() << '"';
    }
    bool is_absolute() const {return data_.front() == preferred_separator;}
    bool is_relative() const {return !is_absolute();}
    const string_type& native() const noexcept {return data_;}
    const value_type* c_str() const noexcept {return data_.c_str();}
    operator string_type() const noexcept {return data_;}
    std::string string() const noexcept {return data_;}
    std::string generic_string() const noexcept {
        std::string copy(data_);
        to_generic(copy);
        return copy;
    }
  private:
#ifdef _WIN32
    void to_native(string_type& data) const {
        std::replace(data.begin(), data.end(), '/', preferred_separator);
    }
    void to_generic(string_type& data) const {
        std::replace(data.begin(), data.end(), preferred_separator, '/');
    }
#else
    void to_native(string_type&) const {}
    void to_generic(string_type&) const {}
#endif
    string_type data_;
};

inline bool create_directory(const path& p) {
    const int status = ::mkdir(p.c_str(), 0755);
    if (status && errno != EEXIST) {
        throw std::runtime_error(p.native());
    }
    return status == 0;
}

inline void current_path(const path& p) {
    if (::chdir(p.c_str())) {
        throw std::runtime_error(p.native());
    }
}

inline path current_path() {
    char buffer[1024];
    if (!::getcwd(buffer, sizeof(buffer))) {
        throw std::runtime_error(buffer);
    }
    return path(buffer);
}

inline bool exists(const path& p) {
    return !::access(p.c_str(), F_OK);
}

} // namespace filesystem
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

// RAII
class ChDir {
  public:
    ChDir(const std::string& dst, bool mkdir=false) {
        if (dst.empty() || dst == ".") return;
        if (mkdir) {
            filesystem::create_directory(dst);
        }
        filesystem::current_path(dst);
    }
    ~ChDir() {
        filesystem::current_path(origin_);
    }
  private:
    const filesystem::path origin_ = filesystem::current_path();
};

} // namespace wtl

#endif /* WTL_FILESYSTEM_HPP_ */

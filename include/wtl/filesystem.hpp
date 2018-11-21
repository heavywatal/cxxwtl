#pragma once
#ifndef WTL_FILESYSTEM_HPP_
#define WTL_FILESYSTEM_HPP_

#if defined(_WIN32)
  #include <direct.h>
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
    path(std::string&& x): data_(std::move(x)) {}

    template <class T>
    path(const T& x): data_(x) {}

    path parent_path() const {
        if (data_ == "/") return *this;
        auto pos = data_.find_last_of('/');
        return path(data_.substr(0u, pos));
    }
    path filename() const {
        if (data_ == "/") return *this;
        auto pos = data_.find_last_of('/');
        return path(data_.substr(++pos));
    }
    path stem() const {
        auto name = filename().string();
        auto pos = name.find_last_of('.');
        if (pos == std::string::npos) return path("");
        if (pos == name.size() - 1u) return path(name);
        return path(name.substr(0u, pos));
    }
    path extension() const {
        auto name = filename().string();
        auto pos = name.find_last_of('.');
        if (pos == std::string::npos) return path("");
        if (pos == name.size() - 1u) return path("");
        return path(name.substr(pos));
    }
    path& append(const path& p) {
        if (p.is_absolute()) {
            data_ = p.data_;
        } else {
            if (data_.back() != '/') {
                data_ += '/';
            }
            data_ += p.string();
        }
        return *this;
    }
    path& operator/=(const path& p) {
        return append(p);
    }
    path& concat(const path& p) {
        data_ += p.string();
        return *this;
    }
    path& operator+=(const path& p) {
        return concat(p);
    }
    friend path operator/(const path& lhs, const path& rhs) {
        return path(lhs.string()) /= rhs;
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
    bool is_absolute() const {return data_[0u] == '/';}
    bool is_relative() const {return !is_absolute();}
    std::string string() const noexcept {return data_;}
  private:
    std::string data_;
};

inline bool create_directory(const path& p) {
#if defined(_WIN32)
    const int status = ::_mkdir(p.string().c_str());
#else
    const int status = ::mkdir(p.string().c_str(), 0755);
#endif
    if (status && errno != EEXIST) {
        throw std::runtime_error(p.string());
    }
    return status == 0;
}

inline void current_path(const path& p) {
#if defined(_WIN32)
    if (::_chdir(p.string().c_str())) {
#else
    if (::chdir(p.string().c_str())) {
#endif
        throw std::runtime_error(p.string());
    }
}

inline path current_path() {
    char buffer[1024];
#if defined(_WIN32)
    if (!::_getcwd(buffer, sizeof(buffer))) {
#else
    if (!::getcwd(buffer, sizeof(buffer))) {
#endif
        throw std::runtime_error(buffer);
    }
    return path(std::string(buffer));
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

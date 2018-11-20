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
#include <regex>

namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace filesystem {

class path {
  public:
    path(std::string&& x): data_(std::move(x)) {}

    template <class T>
    path(const T& x): data_(x) {}

    path parent_path() const {
        std::regex patt("/[^/]*$");
        std::string fmt = "";
        return path(std::regex_replace(data_, patt, fmt));
    }
    path filename() const {
        std::smatch mobj;
        std::regex patt("[^/]*$");
        std::regex_search(data_, mobj, patt);
        return path(mobj.str(0));
    }
    path stem() const {
        std::regex patt("\\.[^.]*$");
        std::string fmt = "";
        return path(std::regex_replace(data_, patt, fmt)).filename();
    }
    path extension() const {
        std::smatch mobj;
        std::regex patt("\\.[^.]*$");
        std::regex_search(data_, mobj, patt);
        return path(mobj.str(0));
    }
    friend bool operator==(const path& lhs, const path& rhs) noexcept {
        return lhs.data_ == rhs.data_;
    }
    friend bool operator!=(const path& lhs, const path& rhs) noexcept {
        return lhs.data_ != rhs.data_;
    }
    friend std::ostream& operator<<(std::ostream& ost, const path& p) noexcept {
        return ost << '"' << p.string() << '"';
    }
    std::string string() const {return data_;}
  private:
    const std::string data_;
};

inline bool create_directory(const std::string& path) {
#if defined(_WIN32)
    const int status = ::_mkdir(path.c_str());
#else
    const int status = ::mkdir(path.c_str(), 0755);
#endif
    if (status && errno != EEXIST) {
        throw std::runtime_error(path);
    }
    return status == 0;
}

inline void current_path(const std::string& path) {
#if defined(_WIN32)
    if (::_chdir(path.c_str())) {
#else
    if (::chdir(path.c_str())) {
#endif
        throw std::runtime_error(path);
    }
}

inline std::string current_path() {
    char buffer[1024];
#if defined(_WIN32)
    if (!::_getcwd(buffer, sizeof(buffer))) {
#else
    if (!::getcwd(buffer, sizeof(buffer))) {
#endif
        throw std::runtime_error(buffer);
    }
    return std::string(buffer);
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
    const std::string origin_ = filesystem::current_path();
};

} // namespace wtl

#endif /* WTL_FILESYSTEM_HPP_ */

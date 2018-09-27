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

namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace filesystem {

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

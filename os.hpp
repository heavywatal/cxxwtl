// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef OS_HPP_
#define OS_HPP_

#include <unistd.h> // chdir, getcwd, unlink, access, getpid, gethostname
#include <dirent.h> // opendir
#include <sys/stat.h> // mkdir, stat

#include <cerrno>
#include <cstdio> // remove
#include <cstdlib> // getenv

#include <string>
#include <fstream>
#include <vector>

#include "debug.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline std::string basename(const std::string& path) {
    size_t pos = path.find_last_of('/');
    return path.substr(++pos);
}

inline std::string dirname(const std::string& path) {
    size_t pos = path.find_last_of('/');
    return path.substr(0, pos);
}

inline bool exists(const std::string& target) {return !::access(target.c_str(), F_OK);}
inline bool can_read(const std::string& target) {return !::access(target.c_str(), R_OK);}
inline bool can_write(const std::string& target) {return !::access(target.c_str(), W_OK);}

inline bool isfile(const std::string& target) {
    struct stat buf;
    return !::stat(target.c_str(), &buf) && S_ISREG(buf.st_mode);
}

inline bool isdir(const std::string& target) {
    struct stat buf;
    return !::stat(target.c_str(), &buf) && S_ISDIR(buf.st_mode);
}

inline std::string getenv(const std::string& s) {
    const char* cstr = std::getenv(s.c_str());
    if (cstr==nullptr) {return std::string{};}
    return std::string(cstr);
}

inline std::string gethostname() {
    constexpr unsigned int bufsize = 80;
    char hostname[bufsize];
    ::gethostname(hostname, bufsize);
    return std::string(hostname);
}

inline std::string pwd() {
    char dir[1024];
    if (::getcwd(dir, sizeof(dir))==nullptr) {FLPF; throw wtl::strerror();}
    return std::string(dir);
}
#define PWD derr("pwd: " << wtl::pwd() << std::endl)

inline void cd(const std::string& dir) {
    if (::chdir(dir.c_str())) {FLPF; throw wtl::strerror(dir);}
}

inline std::vector<std::string>
ls(const std::string& dir, const bool all=false) {
    DIR* dirp = opendir(dir.c_str());
    if (dirp==nullptr) {FLPF; throw strerror(dir);}
    struct dirent* entp;
    std::vector<std::string> dst;
    while ((entp=readdir(dirp))) {
        // ignore '.' and '..'
        if (!strcmp(entp->d_name, ".\0") || !strcmp(entp->d_name, "..\0")) continue;
        // other dot files are depending on the flag
        if (*(entp->d_name)== '.' && !all) continue;
        dst.push_back(entp->d_name);
    }
    closedir(dirp);
    return dst;
}

inline void rm(const std::string& target) {
    if (std::remove(target.c_str())) {FLPF; throw wtl::strerror(target);}
}

inline int mv(const std::string& src, std::string dst, const bool force=false) {
    if (isdir(dst)) {
        dst = dst + "/" + basename(src);
    }
    if (exists(dst) && !force) {FLPF; throw wtl::strerror(dst);}
    const int status(rename(src.c_str(), dst.c_str()));
    if (status) {FLPF; throw wtl::strerror(src+", "+dst);}
    return status;
}

inline void ln(const std::string& src, const std::string& dst, const bool force=false) {
    if (exists(dst) && force) {
        rm(dst);
    }
    if (symlink(src.c_str(), dst.c_str())) {
        FLPF; throw wtl::strerror(dst);
    }
}

inline int mkdir(const std::string& dir) {
    const int status(::mkdir(dir.c_str(), 0755));
    if (status && errno!=EEXIST) {FLPF; throw wtl::strerror(dir);}
    return status;
}

class Pushd{
  public:
    Pushd() {}
    explicit Pushd(const std::string& dst) {operator()(dst);}
    ~Pushd() {HERE;
        while (!stack_.empty()) {popd();}
    }
    void operator()(const std::string& dst) {HERE;
        if (dst.empty()) return;
        stack_.push_back(pwd());
        cd(dst); PWD;
    }
    void popd() {HERE;
        cd(stack_.back()); PWD;
        stack_.pop_back();
    }
  private:
    std::vector<std::string> stack_;
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
// random seed generator

inline unsigned int dev_urandom_handmade() {
    unsigned int x;
    try {
        std::ifstream fin("/dev/urandom", std::ios::binary | std::ios::in);
        fin.exceptions(std::ios::failbit | std::ios::badbit);
        fin.read(reinterpret_cast<char*>(&x), sizeof(x));
    }
    catch (std::ios::failure& e) {throw std::ios::failure("/dev/urandom");}
    return x;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* OS_HPP_ */

// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_FILESYSTEM_HPP_
#define WTL_FILESYSTEM_HPP_

#include "debug.hpp"

#include <boost/filesystem.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace fs = boost::filesystem;

class ChDir {
  public:
    ChDir(const fs::path& dst, const bool mkdir=false) {HERE;
        if (dst.empty() || dst == ".") return;
        if (mkdir) {
            fs::create_directory(dst);
        }
        fs::current_path(dst);
        DCERR("pwd: " << fs::current_path() << std::endl);
    }
    ~ChDir() {HERE;
        fs::current_path(origin_);
        DCERR("pwd: " << fs::current_path() << std::endl);
    }
  private:
    const fs::path origin_ = fs::current_path();
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_FILESYSTEM_HPP_ */

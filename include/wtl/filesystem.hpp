#pragma once
#ifndef WTL_FILESYSTEM_HPP_
#define WTL_FILESYSTEM_HPP_

#include <filesystem>

namespace wtl {

namespace fs = std::filesystem;

// RAII
class ChDir {
  public:
    ChDir(const fs::path& dst, bool mkdir=false) {
        if (dst.empty() || dst == ".") return;
        if (mkdir) {
            fs::create_directory(dst);
        }
        fs::current_path(dst);
    }
    ~ChDir() {
        fs::current_path(origin_);
    }
  private:
    const fs::path origin_ = fs::current_path();
};

} // namespace wtl

#endif /* WTL_FILESYSTEM_HPP_ */

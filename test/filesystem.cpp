#include "filesystem.hpp"
#include "exception.hpp"
#include <iostream>

namespace fs = wtl::filesystem;

int main() {
    fs::create_directory("filesystem");
    auto path0 = fs::current_path();
    std::cout << "pwd: " << path0 << std::endl;
    fs::current_path("filesystem");
    auto path1 = fs::current_path();
    std::cout << "pwd: " << path1 << std::endl;
    WTL_ASSERT(path0 != path1);
    {
      wtl::ChDir cd("chdir", true);
      WTL_ASSERT(fs::current_path() != path1);
    }
    WTL_ASSERT(fs::current_path() == path1);
    return 0;
}

#include <wtl/filesystem.hpp>
#include <wtl/exception.hpp>
#include <iostream>

namespace fs = wtl::filesystem;

void test_path() {
    fs::path p("/path/to/file.tar.gz");
    WTL_ASSERT(p.parent_path().string() == "/path/to");
    WTL_ASSERT(p.filename().string() == "file.tar.gz");
    WTL_ASSERT(p.stem().string() == "file.tar");
    WTL_ASSERT(p.extension().string() == ".gz");
}

int main() {
    test_path();
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

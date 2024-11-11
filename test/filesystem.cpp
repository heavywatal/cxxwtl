#include <wtl/filesystem.hpp>
#include <wtl/exception.hpp>

inline void test_functions() {
    namespace fs = std::filesystem;
    auto path0 = fs::current_path();
    fs::create_directory("filesystem");
    WTL_ASSERT(fs::exists("filesystem"));
    fs::current_path("filesystem");
    auto path1 = fs::current_path();
    WTL_ASSERT(path1.parent_path() == path0);
    {
      wtl::ChDir cd("chdir", true);
      WTL_ASSERT(fs::current_path().parent_path() == path1);
    }
    WTL_ASSERT(fs::current_path() == path1);
}

int main() {
    test_functions();
    return 0;
}

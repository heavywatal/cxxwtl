#include <wtl/filesystem.hpp>
#include <wtl/exception.hpp>

namespace fs = wtl::filesystem;

inline void test_parent_path() {
    WTL_ASSERT(fs::path("/dir/file.txt").parent_path().string() == "/dir");
    WTL_ASSERT(fs::path("/dir/sub/").parent_path().string() == "/dir/sub");
    WTL_ASSERT(fs::path("/").parent_path().string() == "/");
}

inline void test_filename() {
    WTL_ASSERT(fs::path("/dir/file.txt").filename().string() == "file.txt");
    WTL_ASSERT(fs::path("/dir/sub/").filename().string() == "");
    WTL_ASSERT(fs::path("/").filename().string() == "/");
    WTL_ASSERT(fs::path(".").filename().string() == ".");
    WTL_ASSERT(fs::path("..").filename().string() == "..");
}

inline void test_stem() {
    WTL_ASSERT(fs::path("/dir/file.txt").stem().string() == "file");
    WTL_ASSERT(fs::path("/dir/file.tar.gz").stem().string() == "file.tar");
    WTL_ASSERT(fs::path("/dir/").stem().string() == "");
    WTL_ASSERT(fs::path("/dir/.").stem().string() == ".");
    WTL_ASSERT(fs::path("..").stem().string() == "..");
}

inline void test_extension() {
    WTL_ASSERT(fs::path("/dir/file.txt").extension().string() == ".txt");
    WTL_ASSERT(fs::path("/dir/file.tar.gz").extension().string() == ".gz");
    WTL_ASSERT(fs::path("/dir/").extension().string() == "");
    WTL_ASSERT(fs::path("/dir/.").extension().string() == "");
    WTL_ASSERT(fs::path("..").extension().string() == "");
}

inline void test_append() {
    WTL_ASSERT(fs::path("dir") / "file.txt" == "dir/file.txt");
    WTL_ASSERT(fs::path("dir/") / "file.txt" == "dir/file.txt");
    WTL_ASSERT(fs::path("dir") / "" == "dir/");
    WTL_ASSERT(fs::path("dir") / "/file.txt" == "/file.txt");
}

inline void test_functions() {
    auto path0 = fs::current_path();
    fs::create_directory("filesystem");
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
    test_parent_path();
    test_filename();
    test_stem();
    test_extension();
    test_append();
    test_functions();
    return 0;
}

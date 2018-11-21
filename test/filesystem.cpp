#include <wtl/filesystem.hpp>
#include <wtl/exception.hpp>

namespace fs = wtl::filesystem;

inline void test_parent_path() {
    WTL_ASSERT(fs::path("/dir/file.txt").parent_path().string() == "/dir");
    WTL_ASSERT(fs::path("/dir/.").parent_path().string() == "/dir");
    WTL_ASSERT(fs::path("/dir/..").parent_path().string() == "/dir");
    WTL_ASSERT(fs::path("/dir/").parent_path().string() == "/dir");
    WTL_ASSERT(fs::path("/dir").parent_path().string() == "/");
    WTL_ASSERT(fs::path("/").parent_path().string() == "/");
    WTL_ASSERT(fs::path("dir").parent_path().string() == "");
}

inline void test_filename() {
    WTL_ASSERT(fs::path("/dir/file.txt").filename().string() == "file.txt");
    WTL_ASSERT(fs::path("/dir/.").filename().string() == ".");
    WTL_ASSERT(fs::path("/dir/..").filename().string() == "..");
    WTL_ASSERT(fs::path("/dir/").filename().string() == "");
    WTL_ASSERT(fs::path("/dir").filename().string() == "dir");
    WTL_ASSERT(fs::path("/").filename().string() == "");
    WTL_ASSERT(fs::path("dir").filename().string() == "dir");
}

inline void test_stem() {
    WTL_ASSERT(fs::path("/dir/file.txt").stem().string() == "file");
    WTL_ASSERT(fs::path("/dir/file.tar.gz").stem().string() == "file.tar");
    WTL_ASSERT(fs::path("/dir/file").stem().string() == "file");
    WTL_ASSERT(fs::path("/dir/.dotfile").stem().string() == ".dotfile");
    WTL_ASSERT(fs::path("/dir/.").stem().string() == ".");
    WTL_ASSERT(fs::path("/dir/..").stem().string() == "..");
    WTL_ASSERT(fs::path("/dir/").stem().string() == "");
}

inline void test_extension() {
    WTL_ASSERT(fs::path("/dir/file.txt").extension().string() == ".txt");
    WTL_ASSERT(fs::path("/dir/file.tar.gz").extension().string() == ".gz");
    WTL_ASSERT(fs::path("/dir/file").extension().string() == "");
    WTL_ASSERT(fs::path("/dir/.dotfile").extension().string() == "");
    WTL_ASSERT(fs::path("/dir/.").extension().string() == "");
    WTL_ASSERT(fs::path("/dir/..").extension().string() == "");
    WTL_ASSERT(fs::path("/dir/").extension().string() == "");
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
    test_parent_path();
    test_filename();
    test_stem();
    test_extension();
    test_append();
    test_functions();
    return 0;
}

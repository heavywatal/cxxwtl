#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <wtl/filesystem.hpp>
#include <wtl/exception.hpp>

namespace fs = wtl::filesystem;

inline void test_parent_path() {
    auto fun = [](const char* x)
      {return fs::path(x).parent_path().generic_string();};
    WTL_ASSERT(fun("/dir/file.txt") == "/dir");
    WTL_ASSERT(fun("/dir/.") == "/dir");
    WTL_ASSERT(fun("/dir/..") == "/dir");
    WTL_ASSERT(fun("/dir/") == "/dir");
    WTL_ASSERT(fun("/dir") == "/");
    WTL_ASSERT(fun("/") == "/");
    WTL_ASSERT(fun("dir") == "");
}

inline void test_filename() {
    auto fun = [](const char* x)
      {return fs::path(x).filename().generic_string();};
    WTL_ASSERT(fun("/dir/file.txt") == "file.txt");
    WTL_ASSERT(fun("/dir/.") == ".");
    WTL_ASSERT(fun("/dir/..") == "..");
    WTL_ASSERT(fun("/dir/") == "");
    WTL_ASSERT(fun("/dir") == "dir");
    WTL_ASSERT(fun("/") == "");
    WTL_ASSERT(fun("dir") == "dir");
}

inline void test_stem() {
    auto fun = [](const char* x)
      {return fs::path(x).stem().generic_string();};
    WTL_ASSERT(fun("/dir/file.txt") == "file");
    WTL_ASSERT(fun("/dir/file.tar.gz") == "file.tar");
    WTL_ASSERT(fun("/dir/file") == "file");
    WTL_ASSERT(fun("/dir/.dotfile") == ".dotfile");
    WTL_ASSERT(fun("/dir/.") == ".");
    WTL_ASSERT(fun("/dir/..") == "..");
    WTL_ASSERT(fun("/dir/") == "");
}

inline void test_extension() {
    auto fun = [](const char* x)
      {return fs::path(x).extension().generic_string();};
    WTL_ASSERT(fun("/dir/file.txt") == ".txt");
    WTL_ASSERT(fun("/dir/file.tar.gz") == ".gz");
    WTL_ASSERT(fun("/dir/file") == "");
    WTL_ASSERT(fun("/dir/.dotfile") == "");
    WTL_ASSERT(fun("/dir/.") == "");
    WTL_ASSERT(fun("/dir/..") == "");
    WTL_ASSERT(fun("/dir/") == "");
}

inline void test_append() {
    auto fun = [](const char* lhs, const char* rhs)
      {return (fs::path(lhs) / rhs).generic_string();};
    WTL_ASSERT(fun("dir", "file.txt") == "dir/file.txt");
    WTL_ASSERT(fun("dir/", "file.txt") == "dir/file.txt");
    WTL_ASSERT(fun("dir", "") == "dir/");
    WTL_ASSERT(fun("dir", "/file.txt") == "/file.txt");
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

#include <wtl/zlib.hpp>
#include <wtl/exception.hpp>

#include <cerrno>
#include <sstream>

void write_read() {
    std::ostringstream original;
    {
        std::ifstream ifs("../../test/zlib.cpp");
        original << ifs.rdbuf();
    }
    wtl::zlib::ofstream("zlib.cpp.gz") << original.str();
    std::ostringstream archived;
    {
        wtl::zlib::ifstream ifs("zlib.cpp.gz");
        archived << ifs.rdbuf();
    }
    WTL_ASSERT(original.str() == archived.str());

    wtl::zlib::ostringstream zoss;
    zoss << original.str();
    {
        std::ofstream{"zlib.cpp.zoss.gz"} << zoss.str();
        std::ifstream ifs{"zlib.cpp.zoss.gz"};
        archived.str("");
        archived << ifs.rdbuf();
    }
    WTL_ASSERT(zoss.str() == archived.str());
    WTL_ASSERT(2 * zoss.str().size() < original.str().size());
}

void exception() {
    bool exception_thrown = false;
    try {
        wtl::zlib::ifstream ifs("noexist.gz");
    } catch (const std::ios_base::failure& e) {
        exception_thrown = true;
    } catch (const std::exception& e) {// gcc bug
        exception_thrown = true;
    }
    WTL_ASSERT(exception_thrown);
    WTL_ASSERT(errno == ENOENT);
}

int main() {
    write_read();
    exception();
    return 0;
}

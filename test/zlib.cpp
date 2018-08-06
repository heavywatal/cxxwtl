#include "zlib.hpp"
#include "exception.hpp"

#include <cerrno>
#include <iostream>

int main() {
    {
      std::ifstream ifs("../../test/zlib.cpp");
      wtl::zlib::ofstream ofs("zlib.cpp.gz");
      ofs << ifs.rdbuf();
    }
    {
      wtl::zlib::ifstream ifs("zlib.cpp.gz");
      std::ostringstream oss;
      oss << ifs.rdbuf();
      std::cout << oss.str();
    }
    try {
      wtl::zlib::ifstream ifs("noexist.gz");
    } catch (std::ios_base::failure& e) {
      WTL_ASSERT(errno == ENOENT);
    }
    return 0;
}

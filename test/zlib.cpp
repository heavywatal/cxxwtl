#include "zlib.hpp"

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
    } catch (std::runtime_error& e) {
      std::cout << "expected error: " << e.what() << std::endl;
    }
    return 0;
}

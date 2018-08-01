#include "zlib.hpp"

#include <iostream>

int main() {
    std::ifstream ifs("../../test/zlib.cpp");
    wtl::zlib::ofstream ofs("zlib.cpp.gz");
    ofs << ifs.rdbuf();
    return 0;
}

#include "random.hpp"
#include "exception.hpp"

#include <iostream>

int main() {
    for (size_t i=0; i<6u; ++i) {
        double x = wtl::generate_canonical(wtl::mt64());
        std::cout << x << " ";
        WTL_ASSERT(0.0 <= x && x < 1.0);
    }
    std::cout << "\n";
    wtl::bits64_t min_0x3ff{uint64_t{0x3ff0'0000'0000'0000u}}; // 1.0
    wtl::bits64_t max_0x3ff{uint64_t{0x3fff'ffff'ffff'ffffu}}; // 1.999...
    WTL_ASSERT((min_0x3ff.as_double - 1.0) == 0.0);
    WTL_ASSERT((max_0x3ff.as_double - 1.0) < 1.0);
    std::cout.precision(15);
    std::cout << std::scientific
              << min_0x3ff.as_double - 1.0 << "\n"
              << max_0x3ff.as_double - 1.0 << "\n";
}

#include "string.hpp"
#include "exception.hpp"

#include <iostream>

int main() {
    std::cout << "__cplusplus: " << __cplusplus << std::endl;
    WTL_ASSERT((wtl::split("a b\tc") == std::vector<std::string>{"a", "b", "c"}));
    WTL_ASSERT(wtl::strip("  str  ") == "str");
    WTL_ASSERT(wtl::startswith("prefix", "pre"));
    WTL_ASSERT(!wtl::startswith("prefix", "post"));
    WTL_ASSERT(wtl::endswith("suffix", "fix"));
    WTL_ASSERT(!wtl::endswith("suffix", "suf"));
    return 0;
}
#include "iostr.hpp"
#include "exception.hpp"

int main(int argc, char* argv[]) {
    std::cout << "__cplusplus: " << __cplusplus << std::endl;
    std::cout << wtl::str_join(argv, argv + argc, " ") << std::endl;
    std::vector<int> v;
    std::map<int, double> m;
    v.reserve(6);
    for (int i=0; i<6; ++i) {
        v.push_back(i);
        m[i] = 0.5 * i;
    }
    std::cout << v << std::endl;
    std::cout << m << std::endl;
    std::cout << wtl::setfill0w(3) << 7u << std::endl;
    WTL_ASSERT((wtl::split("a b\tc") == std::vector<std::string>{"a", "b", "c"}));
    WTL_ASSERT(wtl::strip("  str  ") == "str");
    WTL_ASSERT(wtl::startswith("prefix", "pre"));
    WTL_ASSERT(!wtl::startswith("prefix", "post"));
    WTL_ASSERT(wtl::endswith("suffix", "fix"));
    WTL_ASSERT(!wtl::endswith("suffix", "suf"));
    return 0;
}

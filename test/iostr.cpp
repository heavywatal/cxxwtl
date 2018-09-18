#include "iostr.hpp"
#include "exception.hpp"

inline void manipulator() {
    std::cout << wtl::setfill0w(3) << 7u << std::endl;
}

inline void stream_op_for_containers() {
    std::vector<int> v;
    std::map<int, double> m;
    v.reserve(6);
    for (int i=0; i<6; ++i) {
        v.push_back(i);
        m[i] = 0.5 * i;
    }
    std::cout << v << std::endl;
    std::cout << m << std::endl;
}

inline void string_manipulation() {
    WTL_ASSERT((wtl::split("a b\tc") == std::vector<std::string>{"a", "b", "c"}));
    WTL_ASSERT(wtl::strip("  str  ") == "str");
    WTL_ASSERT(wtl::startswith("prefix", "pre"));
    WTL_ASSERT(!wtl::startswith("prefix", "post"));
    WTL_ASSERT(wtl::endswith("suffix", "fix"));
    WTL_ASSERT(!wtl::endswith("suffix", "suf"));
}

inline void read_tuple() {
    std::tuple<int, double, std::string> t;
    wtl::read("42 2.718 hello", &t);
    WTL_ASSERT(std::get<0>(t) == 42);
    WTL_ASSERT(std::abs(std::get<1>(t) - 2.718) < 1e-9);
    WTL_ASSERT(std::get<2>(t) == "hello");
}

int main(int argc, char* argv[]) {
    std::cout << "__cplusplus: " << __cplusplus << std::endl;
    wtl::join(argv, argv + argc, std::cout, " ") << std::endl;
    manipulator();
    stream_op_for_containers();
    string_manipulation();
    read_tuple();
    return 0;
}

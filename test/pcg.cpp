#include <wtl/pcg.hpp>
#include <pcg/pcg_random.hpp>

#include <iostream>
#include <random>

namespace wtl {

std::ostream& operator<<(std::ostream& ost, const wtl::pcg32& x) {
    return ost << x.multiplier << " "
               << x.increment_ << " "
               << x.state_;
}

}

int test_eq(wtl::pcg32& origin, pcg32& upstream, unsigned n = 3u) {
    int ret = 0;
    std::cout << origin << std::endl;
    std::cout << upstream << std::endl;
    for (unsigned i=0u; i<n; ++i) {
        const auto expected = upstream();
        const auto observed = origin();
        if (observed != expected) {
            std::cout << observed << " != " << expected << "\n";
            ret = 1;
        }
    }
    return ret;
}

int ctor0() {
    wtl::pcg32 origin;
    pcg32 upstream;
    return test_eq(origin, upstream);
}

int ctor1() {
    wtl::pcg32 origin(42u);
    pcg32 upstream(42u);
    return test_eq(origin, upstream);
}

int ctor2() {
    wtl::pcg32 origin(42u, 54u);
    pcg32 upstream(42u, 54u);
    return test_eq(origin, upstream);
}

int seed1() {
    wtl::pcg32 origin;
    pcg32 upstream;
    origin.seed(42u);
    upstream.seed(42u);
    return test_eq(origin, upstream);
}

int seed2() {
    wtl::pcg32 origin;
    pcg32 upstream;
    origin.seed(42u, 54u);
    upstream.seed(42u, 54u);
    return test_eq(origin, upstream);
}

int seedseq() {
    std::seed_seq sseq{42u, 54u};
    wtl::pcg32 origin;
    pcg32 upstream;
    origin.seed(sseq);
    upstream.seed(sseq);
    return test_eq(origin, upstream);
}

int main() {
    int ret = 0;
    ret |= ctor0();
    ret |= ctor1();
    ret |= ctor2();
    ret |= seed1();
    ret |= seed2();
    if (false) {// TODO
      ret |= seedseq();
    }
    return ret;
}

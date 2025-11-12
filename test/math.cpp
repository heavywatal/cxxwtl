#include <wtl/math.hpp>
#include <wtl/exception.hpp>

int main() {
    WTL_ASSERT(wtl::pow(2, 0) == 1);
    WTL_ASSERT(wtl::pow(2, 1) == 2);
    WTL_ASSERT(wtl::pow(2, 8) == 256);
    WTL_ASSERT(wtl::factorial(0) == 1);
    WTL_ASSERT(wtl::factorial(5) == 120);
    WTL_ASSERT(wtl::permut(5, 2) == 20);
    WTL_ASSERT(wtl::choose(5, 2) == 10);
    WTL_ASSERT(wtl::pow(2u, 8u) == 256u);
    WTL_ASSERT(wtl::factorial(5u) == 120u);
    WTL_ASSERT(wtl::permut(5u, 2u) == 20u);
    WTL_ASSERT(wtl::choose(5u, 2u) == 10u);
    return 0;
}

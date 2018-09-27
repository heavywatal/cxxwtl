#include <wtl/math.hpp>
#include <wtl/exception.hpp>

int main() {
    WTL_ASSERT(wtl::pow(2u, 0u) == 1u);
    WTL_ASSERT(wtl::pow(2u, 1u) == 2u);
    WTL_ASSERT(wtl::pow(2u, 8u) == 256u);
    WTL_ASSERT(wtl::factorial(0u) == 1u);
    WTL_ASSERT(wtl::factorial(5u) == 120u);
    WTL_ASSERT(wtl::permut(5u, 2u) == 20u);
    WTL_ASSERT(wtl::choose(5u, 2u) == 10u);
    return 0;
}

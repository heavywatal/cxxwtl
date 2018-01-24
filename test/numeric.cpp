#include "numeric.hpp"
#include "exception.hpp"
#include "iostr.hpp"

inline void test_integral() {
    constexpr double pi = 3.14159265358979323846;
    WTL_ASSERT(wtl::approx(1.0,
        wtl::integrate([](double x){return 1.0 + x * 0.0;}, 0.0, 1.0)));
    WTL_ASSERT(wtl::approx(0.5,
        wtl::integrate([](double x){return x;}, 0.0, 1.0)));
    WTL_ASSERT(wtl::approx(2.0,
        wtl::integrate([](double x){return std::sin(x);}, 0.0, pi), 1e-7));
    WTL_ASSERT(wtl::approx(0.0,
        wtl::integrate([](double x){return std::cos(x);}, 0.0, pi), 1e-14));
    WTL_ASSERT(wtl::approx(pi / 4.0,
        wtl::integrate([](double x){return std::sqrt(1 - x * x);}, 0.0, 1.0), 1e-3));
    WTL_ASSERT(wtl::approx(0.5 * std::sqrt(pi),
        wtl::integrate([](double x){return std::exp(- x * x);}, 0.0, 10.0)));
}

inline void test_valarray() {
    std::cout.precision(16);
    std::cout << wtl::lin_spaced(11) << std::endl;
    std::cout << wtl::round(wtl::lin_spaced(11), 100) << std::endl;
    std::cout << wtl::lin_spaced(51) << std::endl;
    std::cout << wtl::round(wtl::lin_spaced(51), 100) << std::endl;
}

int main() {
    test_integral();
    test_valarray();
    return 0;
}

#include "numeric.hpp"
#include "iostr.hpp"

inline void test_integral() {
    constexpr double pi = 3.14159265358979323846;
    std::cerr << "y = 1: "
        << wtl::integrate([](const double x){return 1.0 + x*0;}, 0, 1) << std::endl;
    std::cerr << "y = x: "
        << wtl::integrate([](const double x){return x;}, 0, 1) << std::endl;
    std::cerr << "y = sin(x): "
        << wtl::integrate([](const double x){return std::sin(x);}, 0, pi) << std::endl;
    std::cerr << "y = cos(x): "
        << wtl::integrate([](const double x){return std::cos(x);}, 0, pi) << std::endl;
    std::cerr << "x^2 + y^2 = 1: "
        << wtl::integrate([](const double x){return std::sqrt(1 - x * x);}, 0, 1) * 4 << std::endl;
    std::cerr << "y = exp(-x^2) / sqrt(pi): "
        << wtl::integrate([](const double x)
            {return std::exp(- x * x);}, 0, 10, 100) * 2 / std::sqrt(pi) << std::endl;
}

inline void test_numbers() {
    std::cout << std::numeric_limits<size_t>::max() << std::endl;
    std::cout << std::log2(std::numeric_limits<size_t>::max()) << std::endl;
    std::cout << std::log10(std::numeric_limits<size_t>::max()) << std::endl;
    std::cout << wtl::lin_spaced(11) << std::endl;
    std::cout << wtl::round(wtl::lin_spaced(11), 100) << std::endl;
    std::cout << wtl::lin_spaced(51) << std::endl;
    std::cout << wtl::round(wtl::lin_spaced(51), 100) << std::endl;
}

int main() {
    test_integral();
    test_numbers();
}
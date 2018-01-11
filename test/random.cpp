#include "random.hpp"
#include "exception.hpp"

#include <iostream>

int main() {
    for (size_t i=0; i<6u; ++i) {
        double x = wtl::generate_canonical(wtl::mt64());
        std::cout << x << ", ";
        ASSERT_THAT(0.0 < x && x <= 1.0);
    }
}

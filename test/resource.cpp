#include <wtl/resource.hpp>
#include <thread>
#include <iostream>

// to suppress optimization
unsigned global = 0u;

inline void sub(unsigned n, unsigned divisor = 2u, unsigned sleep = 0u) {
    unsigned sum = 0u;
    for (unsigned i = 0u; i < n; ++i) {
        if ((i % divisor) == 0u) {
            sum += i;
        }
    }
    global += sum;

    // takes time, but should not consume CPU time
    if (sleep > 0u) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
    }
}

int main() {
    std::cout << wtl::benchmark([]{sub(400000u, 1000u);}, "i % 1000", 2u);
    std::cout << wtl::benchmark([]{sub(400000u, 1023u);}, "i % 1023", 2u);
    std::cout << wtl::benchmark([]{sub(400000u, 1024u);}, "i % 1024", 2u);
    std::cout << wtl::benchmark([]{sub(8u, 8u, 80u);}, "sleep", 2u);
    return 0;
}

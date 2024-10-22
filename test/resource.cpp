#include <wtl/resource.hpp>
#include <thread>
#include <iostream>
#include <memory>

// to suppress optimization
unsigned global = 0u;

inline void div(unsigned n, unsigned divisor = 2u) {
    unsigned sum = 0u;
    for (unsigned i = 0u; i < n; ++i) {
        if ((i % divisor) == 0u) {
            sum += i;
        }
    }
    global += sum;
}

template <class T=unsigned>
inline void mem(const unsigned n) {
    std::vector<std::unique_ptr<T>> v;
    v.reserve(n);
    unsigned sum = 0u;
    for (unsigned i = 0u; i < n; ++i) {
        v.push_back(std::make_unique<T>(i));
        sum += *v[i];
    }
    global += sum + static_cast<unsigned>(v.size());
}

int main() {
    using namespace std::literals::chrono_literals;
    // sleep takes time, but should not consume CPU time
    std::cout << wtl::diff_rusage([]{std::this_thread::sleep_for(30ms);}) << "\tsleep" << std::endl;
    // power-of-two is slightly faster
    std::cout << wtl::diff_rusage([]{div(400000u, 1000u);}, 3u) << "\ti % 1022" << std::endl;
    std::cout << wtl::diff_rusage([]{div(400000u, 1023u);}, 3u) << "\ti % 1023" << std::endl;
    std::cout << wtl::diff_rusage([]{div(400000u, 1024u);}, 3u) << "\ti % 1024" << std::endl;
    // memory may be reused and not be returned to OS immediately
    std::cout << wtl::diff_rusage([]{mem(1u * 1024u);}) << "\tvector(1k)" << std::endl;
    std::cout << wtl::diff_rusage([]{mem(32u * 1024u);}) << "\tvector(32k)" << std::endl;
    std::cout << wtl::diff_rusage([]{mem(32u * 1024u);}) << "\tvector(32k)" << std::endl;
    return 0;
}

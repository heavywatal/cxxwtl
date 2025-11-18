#include <wtl/resource.hpp>
#include <wtl/signed.hpp>
#include <wtl/iostr.hpp>

#include <thread>
#include <iostream>
#include <memory>

// to suppress optimization
int global = 0;

inline void divi(const int n, const int divisor = 2) {
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        if ((i % divisor) == 0) {
            sum += i;
        }
    }
    global += sum;
}

template <class T=int>
inline void mem(const int n) {
    std::vector<std::unique_ptr<T>> v;
    wtl::reserve(v, n);
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        v.push_back(std::make_unique<T>(i));
        sum += *wtl::at(v, i);
    }
    global += sum + static_cast<int>(v.size());
}

int main() {
    using namespace std::literals::chrono_literals;
    // sleep takes time, but should not consume CPU time
    std::cout << wtl::delta_rusage([]{std::this_thread::sleep_for(30ms);}) << "\tsleep" << std::endl;
    // power-of-two is slightly faster
    std::cout << wtl::delta_rusage([]{divi(400000, 1000);}, 3) << "\ti % 1022" << std::endl;
    std::cout << wtl::delta_rusage([]{divi(400000, 1023);}, 3) << "\ti % 1023" << std::endl;
    std::cout << wtl::delta_rusage([]{divi(400000, 1024);}, 3) << "\ti % 1024" << std::endl;
    // memory may be reused and not be returned to OS immediately
    std::cout << wtl::delta_rusage([]{mem(1 * 1024);}) << "\tvector(1k)" << std::endl;
    std::cout << wtl::delta_rusage([]{mem(32 * 1024);}) << "\tvector(32k)" << std::endl;
    std::cout << wtl::delta_rusage([]{mem(32 * 1024);}) << "\tvector(32k)" << std::endl;
    return 0;
}

#include <wtl/resource.hpp>
#include <thread>
#include <iostream>

inline void do_something() {
    unsigned sum = 0;
    for (unsigned i = 0; i < 100u; ++i) {
        sum += i;
    }
    std::cout << "sum: " << sum << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main() {
    const auto epoch = wtl::getrusage();
    do_something();
    auto ru = wtl::getrusage<std::micro, std::kilo>(epoch);
    std::cout << wtl::rusage_header() << "\n" << ru << "\n";
    std::cout << "user:   " << ru.utime << "\n";
    std::cout << "system: " << ru.stime << "\n";
    std::cout << "memory: " << ru.maxrss << "\n";
    return 0;
}
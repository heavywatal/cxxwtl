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
    struct rusage ru_start;
    struct rusage ru_end;
    getrusage(RUSAGE_SELF, &ru_start);
    do_something();
    getrusage(RUSAGE_SELF, &ru_end);
    auto utime = wtl::utime<std::micro>(ru_end, ru_start);
    auto stime = wtl::stime<std::micro>(ru_end, ru_start);
    std::cout << "usr: " << utime << "\n";
    std::cout << "sys: " << stime << "\n";
    return 0;
}

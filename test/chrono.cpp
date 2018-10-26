#include <wtl/chrono.hpp>
#include <thread>

int main() {
    using namespace std::literals::chrono_literals;
    wtl::benchmark([](){std::this_thread::sleep_for(30ms);}, "sleep_for(30ms)");
    wtl::benchmark([](){std::this_thread::sleep_for(60ms);}, "sleep_for(60ms)");
    std::cout << "wtl::strftime():        " << wtl::strftime() << "\n";
    std::cout << "wtl::iso8601date():     " << wtl::iso8601date() << "\n";
    std::cout << "wtl::iso8601time():     " << wtl::iso8601time() << "\n";
    std::cout << "wtl::iso8601datetime(): " << wtl::iso8601datetime() << "\n";
    return 0;
}

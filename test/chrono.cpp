#include <wtl/chrono.hpp>
#include <thread>

int main() {
    using namespace std::literals::chrono_literals;
    wtl::stopwatch(std::cout << "#sleep_for(30ms)\n",
      [](){std::this_thread::sleep_for(30ms);});
    wtl::stopwatch<std::chrono::microseconds>(std::cout << "#sleep_for(3ms)\n",
      [](){std::this_thread::sleep_for(3ms);});
    std::cout << "wtl::strftime():        " << wtl::strftime() << "\n";
    std::cout << "wtl::iso8601date():     " << wtl::iso8601date() << "\n";
    std::cout << "wtl::iso8601time():     " << wtl::iso8601time() << "\n";
    std::cout << "wtl::iso8601datetime(): " << wtl::iso8601datetime() << "\n";
    return 0;
}

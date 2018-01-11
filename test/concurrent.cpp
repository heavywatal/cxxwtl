#include "concurrent.hpp"

#include <iostream>
#include <sstream>

int main() {
    wtl::ThreadPool pool(2);
    std::vector<std::future<size_t>> futures;
    for (size_t j=0; j<2ul; ++j) {
        for (size_t i=0; i<3ul; ++i) {
            futures.push_back(pool.submit([](const size_t k) {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                std::ostringstream oss;
                oss << std::this_thread::get_id() << ": " << k << "\n";
                std::cout << oss.str() << std::flush;
                return k;
            }, i));
        }
        std::cout << std::this_thread::get_id() << ": main queued\n" << std::flush;
        pool.wait();
        std::cout << std::this_thread::get_id() << ": main finished\n" << std::flush;
    }
    for (auto& f: futures) {
        std::cout << f.get() << " ";
    }
    return 0;
}

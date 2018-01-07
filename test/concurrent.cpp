#include "concurrent.hpp"
#include "iostr.hpp"

inline void test_semaphore() {
    const unsigned int concurrency = std::thread::hardware_concurrency();
    std::cerr << "std::thread::hardware_concurrency(): "
              << concurrency << std::endl;
    const size_t n = concurrency * 2;

    std::vector<std::future<size_t>> futures;
    wtl::Semaphore semaphore(concurrency);

    for (size_t i=0; i<n; ++i) {
        // manual lock before thread creation: reused from pool
        semaphore.lock();
        futures.push_back(std::async(std::launch::async, [&semaphore, i] {
            std::lock_guard<wtl::Semaphore> scope_unlock(semaphore, std::adopt_lock);
            std::ostringstream oss;
            oss << std::this_thread::get_id() << ": " << i << "\n";
            std::cerr << oss.str() << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            return i;
        }));
    }
    for (auto& x: futures) x.wait();
    for (auto& x: futures) {
        std::cerr << x.get() << " ";
    }
    std::cerr << std::endl;

    futures.clear();
    for (size_t i=0; i<n; ++i) {
        // RAII lock after thread creation: many new threads are created
        futures.push_back(std::async(std::launch::async, [&semaphore, i] {
            std::lock_guard<wtl::Semaphore> _(semaphore);
            std::ostringstream oss;
            oss << std::this_thread::get_id() << ": " << i << "\n";
            std::cerr << oss.str() << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            return i;
        }));
    }
    for (auto& x: futures) x.wait();
    for (auto& x: futures) {
        std::cerr << x.get() << " ";
    }
    std::cerr << std::endl;
}

inline void test_thread_pool() {
    wtl::ThreadPool pool(2);
    std::vector<std::future<size_t>> futures;
    for (size_t j=0; j<2ul; ++j) {
        for (size_t i=0; i<4ul; ++i) {
            futures.push_back(pool.submit([](const size_t k) {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                std::ostringstream oss;
                oss << std::this_thread::get_id() << ": " << k << "\n";
                std::cerr << oss.str() << std::flush;
                return k;
            }, i));
        }
        std::cerr << std::this_thread::get_id() << ": main\n" << std::flush;
        pool.wait();
        std::cerr << std::this_thread::get_id() << ": main\n" << std::flush;
    }
    for (auto& f: futures) {
        std::cout << f.get() << " ";
    }
}

int main() {
    test_semaphore();
    test_thread_pool();
}
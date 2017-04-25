// -*- mode: c++; coding: utf-8 -*-
#include "numeric.hpp"
#include "debug.hpp"
#include "iostr.hpp"
#include "mixin.hpp"
#include "os.hpp"
#include "demangle.hpp"
#include "prandom.hpp"
#include "concurrent.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class Cls: public Singleton<Cls> {
    friend Singleton<Cls>;
  public:
    int get_x() const {return x_;}
    void print_x() const {std::cout << x_ << std::endl;}
  private:
    Cls(const int x): x_(x) {HERE;};
    int x_ = 42;
};

inline void test_integral() {HERE;
    std::cerr << "y = 1: " << wtl::integrate([](const double x){return 1.0 + x*0;}, 0, 1) << std::endl;
    std::cerr << "y = x: " << wtl::integrate([](const double x){return x;}, 0, 1) << std::endl;
    std::cerr << "y = sin(x): "
        << wtl::integrate([](const double x){return std::sin(x);}, 0, M_PI) << std::endl;
    std::cerr << "y = cos(x): "
        << wtl::integrate([](const double x){return std::cos(x);}, 0, M_PI) << std::endl;
    std::cerr << "x^2 + y^2 = 1: "
        << wtl::integrate([](const double x){return std::sqrt(1 - x * x);}, 0, 1) * 4 << std::endl;
    std::cerr << "y = exp(-x^2) / sqrt(pi): "
        << wtl::integrate([](const double x)
            {return std::exp(- x * x);}, 0, 10, 100) * 2 / std::sqrt(M_PI) << std::endl;
}

inline void test_validity() {HERE;
    auto& ref = Cls::instance(1);
    ref.print_x();
    Cls::instance(2).print_x();
}

inline void test_speed() {HERE;
    constexpr size_t n = 2 * 1000 * 1000;
    std::vector<size_t> x(n);

    double mu = 8.0;
    std::poisson_distribution<size_t> dist(mu);

    wtl::benchmark([&](){
        for (size_t j=0; j<n; ++j) {
            x[j] = dist(wtl::sfmt());
        }
    });
    std::cerr << wtl::mean(x) << std::endl;

    wtl::benchmark([&](){
        for (size_t j=0; j<n; ++j) {
            x[j] = dist(wtl::mt());
        }
    });
    std::cerr << wtl::mean(x) << std::endl;

    wtl::benchmark([&](){
        for (size_t j=0; j<n; ++j) {
            x[j] = wtl::prandom().poisson(mu);
        }
    });
    std::cerr << wtl::mean(x) << std::endl;

    size_t k = n / 50;
    size_t trash = 0;
    wtl::benchmark([&](){
        trash += wtl::sample(x, k, wtl::sfmt())[0];
    });
    std::cerr << trash << std::endl;
    wtl::benchmark([&](){
        trash += wtl::sample_set(x, k, wtl::sfmt())[0];
    });
    std::cerr << trash << std::endl;
    wtl::benchmark([&](){
        trash += wtl::sample_fisher(x, k, wtl::sfmt())[0];
    });
    std::cerr << trash << std::endl;
    wtl::benchmark([&](){
        trash += wtl::sample_knuth(x, k, wtl::sfmt())[0];
    });
    std::cerr << trash << std::endl;
    x.resize(6);
    std::cerr << x << std::endl;
}

std::mutex MUTEX;

inline void cxx11_thread() {HERE;
    const size_t concurrency = std::thread::hardware_concurrency();
    std::cerr << "std::thread::hardware_concurrency(): "
              << concurrency << std::endl;
    const size_t n = concurrency * 2;

    std::vector<std::future<size_t>> futures;
    wtl::Semaphore semaphore(concurrency);

    for (size_t i=0; i<n; ++i) {
        // manual lock before thread creation: reused from pool
        semaphore.lock();
        futures.push_back(std::async(std::launch::async, [&semaphore, i] {
            std::ostringstream oss;
            oss << std::this_thread::get_id() << ": " << i << "\n";
            std::cerr << oss.str() << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            semaphore.unlock();
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

inline void test_temporal() {HERE;
    std::cout << std::numeric_limits<size_t>::max() << std::endl;
    std::cout << std::log2(std::numeric_limits<size_t>::max()) << std::endl;
    std::cout << std::log10(std::numeric_limits<size_t>::max()) << std::endl;
    std::cout << wtl::lin_spaced(11) << std::endl;
    std::cout << wtl::round(wtl::lin_spaced(11), 100) << std::endl;
    std::cout << wtl::lin_spaced(51) << std::endl;
    std::cout << wtl::round(wtl::lin_spaced(51), 100) << std::endl;
}

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.precision(16);
    std::cerr.precision(6);
    try {
        std::cerr << wtl::str_join(argv, argv + argc, " ") << std::endl;
        // test_integral();
        // test_validity();
        // test_speed();
        cxx11_thread();
        // test_temporal();
        std::cerr << "EXIT_SUCCESS" << std::endl;
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << "\n" << wtl::typestr(e) << ": " << e.what() << std::endl;
    }
    return EXIT_FAILURE;
}

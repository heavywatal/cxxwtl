#include "numeric.hpp"
#include "debug.hpp"
#include "iostr.hpp"
#include "demangle.hpp"
#include "prandom.hpp"
#include "concurrent.hpp"
#include "genetic.hpp"

#include <sfmt.hpp> // https://github.com/heavywatal/sfmt-class

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline void test_integral() {HERE;
    constexpr double pi = 3.14159265358979323846;
    std::cerr << "y = 1: "
        << wtl::integrate([](const double x){return 1.0 + x*0;}, 0, 1) << std::endl;
    std::cerr << "y = x: "
        << wtl::integrate([](const double x){return x;}, 0, 1) << std::endl;
    std::cerr << "y = sin(x): "
        << wtl::integrate([](const double x){return std::sin(x);}, 0, pi) << std::endl;
    std::cerr << "y = cos(x): "
        << wtl::integrate([](const double x){return std::cos(x);}, 0, pi) << std::endl;
    std::cerr << "x^2 + y^2 = 1: "
        << wtl::integrate([](const double x){return std::sqrt(1 - x * x);}, 0, 1) * 4 << std::endl;
    std::cerr << "y = exp(-x^2) / sqrt(pi): "
        << wtl::integrate([](const double x)
            {return std::exp(- x * x);}, 0, 10, 100) * 2 / std::sqrt(pi) << std::endl;
}

inline void test_speed() {HERE;
    constexpr size_t n = 2 * 1000 * 1000;
    std::vector<size_t> x(n);

    double mu = 8.0;
    std::poisson_distribution<size_t> dist(mu);

    auto lambda_random = [&](auto& generator) mutable {
        for (size_t j=0; j<n; ++j) {
            x[j] = dist(generator);
        }
    };
    wtl::benchmark(std::bind(lambda_random, wtl::mt()), "mt", 2);
    std::cerr << wtl::mean(x) << std::endl;
    wtl::benchmark(std::bind(lambda_random, wtl::mt64()), "mt64", 2);
    std::cerr << wtl::mean(x) << std::endl;
    wtl::benchmark(std::bind(lambda_random, wtl::sfmt()), "sfmt", 2);
    std::cerr << wtl::mean(x) << std::endl;
    wtl::benchmark(std::bind(lambda_random, wtl::sfmt64()), "sfmt64", 2);
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

inline void cxx11_thread() {HERE;
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

inline void test_thread_pool() {HERE;
    wtl::ThreadPool pool(2);
    for (size_t j=0; j<2ul; ++j) {
        for (size_t i=0; i<4ul; ++i) {
            pool.submit([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::ostringstream oss;
                oss << std::this_thread::get_id() << ": " << i << "\n";
                std::cerr << oss.str() << std::flush;
            });
        }
        std::cerr << std::this_thread::get_id() << ": main\n" << std::flush;
        pool.wait();
        std::cerr << std::this_thread::get_id() << ": main\n" << std::flush;
    }
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

inline void test_genetic() {HERE;
    const size_t n = 200'000;
    std::vector<double> fitnesses(n);
    std::vector<double> children(n);

    for (double& w: fitnesses) {w = wtl::sfmt64().canonical();}
    wtl::benchmark([&](){
        for (size_t i=0u; i<10u; ++i) {
            auto indices = wtl::roulette_select_cxx11(fitnesses, n, wtl::sfmt());
            children.clear();
            for (const auto j: indices) {
                children.push_back(fitnesses[j]);
            }
            fitnesses.swap(children);
        }
        std::cerr << wtl::mean(fitnesses) << std::endl;
    });

    for (double& w: fitnesses) {w = wtl::sfmt64().canonical();}
    wtl::benchmark([&](){
        for (size_t i=0u; i<10u; ++i) {
            auto indices = wtl::roulette_select(fitnesses, n, wtl::sfmt());
            children.clear();
            for (const auto j: indices) {
                children.push_back(fitnesses[j]);
            }
            fitnesses.swap(children);
        }
        std::cerr << wtl::mean(fitnesses) << std::endl;
    });

    for (double& w: fitnesses) {w = wtl::sfmt64().canonical();}
    wtl::benchmark([&](){
        for (size_t i=0u; i<10u; ++i) {
            std::discrete_distribution<size_t> dist(fitnesses.begin(), fitnesses.end());
            children.clear();
            for (size_t j=0u; j<n; ++j) {
                children.push_back(fitnesses[dist(wtl::sfmt())]);
            }
            fitnesses.swap(children);
        }
        std::cerr << wtl::mean(fitnesses) << std::endl;
    });
}

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.precision(16);
    std::cerr.precision(6);
    try {
        std::cerr << wtl::str_join(argv, argv + argc, " ") << std::endl;
        // test_integral();
        // test_speed();
        // cxx11_thread();
        // test_thread_pool();
        test_genetic();
        // test_temporal();
        std::cerr << "EXIT_SUCCESS" << std::endl;
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << "\n" << wtl::typestr(e) << ": " << e.what() << std::endl;
    }
    return EXIT_FAILURE;
}

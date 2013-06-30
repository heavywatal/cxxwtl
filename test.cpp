// -*- mode: c++; coding: utf-8 -*-

#include <iostream>

#include <chrono>
#include <random>
#include <regex>

#include "algorithm.hpp"
#include "cow.hpp"
#include "debug.hpp"
#include "genetic.hpp"
//#include "getopt.hpp" // needs boost/program_options
//#include "grn.hpp" // needs boost/graph
//#include "gz.hpp" // needs boost/iostreams
#include "iostr.hpp"
#include "mixin.hpp"
#include "omp.hpp"
#include "os.hpp"
#include "prandom.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class Cls: public Singleton<Cls> {
    friend Singleton<Cls>;
  public:
    int get_x() const {return x_;}
    void print_x() const {std::cout << x_ << std::endl;}
  private:
    Cls(): x_(42) {};
    Cls(const int x): x_(x) {};
    int x_;
};

inline void test_validity() {
    auto& ref = Cls::instance(1);
    ref.print_x();
    Cls::instance(2).print_x();
}

inline void test_speed() {
    const size_t n = 1000000;
    std::vector<double> x(n);

    wtl::Fout dev_null("/dev/null");

    double mu = 0.01;
    auto dist = std::normal_distribution<>(mu);

    wtl::benchmark([&](){
        for (size_t j=0; j<n; ++j) {
            x[j] = prandom().normal(mu);
        }
    });
    std::cerr << wtl::mean(x) << std::endl;

    wtl::benchmark([&](){
        auto gen = rng(dist);
        for (size_t j=0; j<n; ++j) {
            x[j] = gen();
        }
    });
    std::cerr << wtl::mean(x) << std::endl;

    wtl::benchmark([&](){
        auto gen = std_rng(dist);
        for (size_t j=0; j<n; ++j) {
            x[j] = gen();
        }
    });
    std::cerr << wtl::mean(x) << std::endl;

//    wtl::benchmark([&]() {
//        ;
//    });

    x.resize(6);
    std::cerr << x << std::endl;
}

inline void cxx11_regex() {
    std::regex patt{"(\\w+)(file)"};
    std::smatch match;
    auto entries = wtl::ls(".");
    for (const auto& entry: entries) {
        if (std::regex_search(entry, match, patt)) {
            for (const auto& sm: match) {
                std::cerr << sm << std::endl;
            }
        }
    }
}

inline void test_function() {HERE;
    const std::string homedir(std::getenv("HOME"));
    const std::string tmpdir = homedir + "/tmp";
    wtl::mkdir(tmpdir);

    std::cerr << "hostname: " << wtl::gethostname() << std::endl;
    std::cerr << "pid: " << ::getpid() << std::endl;
    std::cerr << "pwd: " << wtl::pwd() << std::endl;
    std::cerr << wtl::LINE << wtl::strftime() << "\n" << wtl::LINE << std::endl;

    test_validity();
    test_speed();
    cxx11_regex();
}


int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.precision(16);
    std::cerr.precision(6);
    
    try {
        std::cerr << "argc: " << argc << std::endl;
        std::cerr << "argv: " << wtl::str_join(argv, argv + argc, " ") << std::endl;
        test_function();
        
        std::cerr << "EXIT_SUCCESS" << std::endl;
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << "\n" << wtl::typestr(e) << ": " << e.what() << std::endl;
    }
    catch (const char* const e) {std::cerr << "\nEXCEPTION:\n" << e << std::endl;}
    catch (const char* e) {std::cerr << "\nEXCEPTION:\n" << e << std::endl;}
    catch (const int e) {
        std::cerr << "\nint " << e << " was thrown.\nIf it is errno: "
                  << std::strerror(e) << std::endl;
    }
    catch (...) {
        std::cerr << "\nUNKNOWN ERROR OCCURED!!\nerrno "
                  << errno << ": " << std::strerror(errno) << std::endl;
    }
    
    return EXIT_FAILURE;
}

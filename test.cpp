// -*- mode: c++; coding: utf-8 -*-

#include <iostream>

#include <chrono>
#include <random>
#include <regex>

#include "debug.hpp"
#include "iostr.hpp"
#include "prandom.hpp"
#include "algorithm.hpp"
#include "genetic.hpp"
#include "os.hpp"
#include "cow.hpp"
#include "omp.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

size_t num_items = 1000000;

// http://enki-tech.blogspot.jp/2012/08/c11-generic-singleton.html
template <class T>
class Singleton {
  public:
    template <typename... Args>
    static T* get_instance(Args... args) {
        if (!instance_) {
            instance_ = new T(std::forward<Args>(args)...);
        }
        return instance_;
    }
    
    static void destroy_instance() {
        delete instance_;
        instance_ = nullptr;
    }
    
  private:
    static T* instance_;
};
template <class T> T*  Singleton<T>::instance_ = nullptr;

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

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline void test_function() {HERE;
    const std::string homedir(std::getenv("HOME"));
    const std::string tmpdir = homedir + "/tmp";
    wtl::mkdir(tmpdir);

    std::cerr << "hostname: " << wtl::gethostname() << std::endl;
    std::cerr << "pid: " << ::getpid() << std::endl;
    std::cerr << "pwd: " << wtl::pwd() << std::endl;
    std::cerr << wtl::LINE << wtl::strftime() << "\n" << wtl::LINE << std::endl;

    wtl::Fout dev_null("/dev/null");

    const size_t n = num_items;
    std::vector<double> x(n);

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

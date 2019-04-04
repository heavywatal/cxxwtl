#include <wtl/iostr.hpp>
#include <wtl/exception.hpp>

inline void manipulator() {
    std::cout << wtl::setfill0w(3) << 7u << std::endl;
}

inline void stream_op_for_containers() {
    std::vector<int> v;
    std::map<int, double> m;
    v.reserve(6);
    for (int i=0; i<6; ++i) {
        v.push_back(i);
        m[i] = 0.5 * i;
    }
    std::cout << v << std::endl;
    std::cout << m << std::endl;
}

inline void read_array() {
    std::tuple<int, double, std::string> t;
    wtl::read("42 2.718 hello", &t);
    WTL_ASSERT(std::get<0>(t) == 42);
    WTL_ASSERT(std::abs(std::get<1>(t) - 2.718) < 1e-9);
    WTL_ASSERT(std::get<2>(t) == "hello");
    std::vector<int> v;
    wtl::read("-1 0 1", &v);
    WTL_ASSERT(v.at(0u) == -1);
    WTL_ASSERT(v.at(1u) == 0);
    WTL_ASSERT(v.at(2u) == 1);
}

inline void read_lines() {
    std::istringstream issv("1 2 3\n4 5 6\n");
    WTL_ASSERT(wtl::readlines<std::vector<int>>(issv) ==
        (std::vector<std::vector<int>>{{1, 2, 3}, {4, 5, 6}}));

    std::istringstream iss("1 2 3\n4 5 6\n");
    WTL_ASSERT(wtl::readlines(iss) ==
        (std::vector<std::string>{"1 2 3", "4 5 6"}));
}

inline void null_stream() {
    wtl::null_ostream nost;
    nost << "Error: this must no be printed\n";
}

int main(int argc, char* argv[]) {
    std::cout << "__cplusplus: " << __cplusplus << std::endl;
    wtl::join(argv, argv + argc, std::cout, " ") << std::endl;
    manipulator();
    stream_op_for_containers();
    read_array();
    read_lines();
    null_stream();
    return 0;
}

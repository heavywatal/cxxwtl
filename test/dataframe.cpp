#include <wtl/dataframe.hpp>
#include <wtl/iostr.hpp>

int main() {
    wtl::DataFrame df;
    df.add_column<int>("id");
    df.add_column<double>("value");
    df.add_column<std::string>("label");
    df.reserve(8u);
    df.append(1, 0.1, std::string("a"));
    df.append(2, 0.2, std::string("b"));
    df.append(3, 0.3, std::string("c"));
    std::cout << df;
    std::cout << df.at<int>(0u) << std::endl;
    std::cout << df.at<double>(1u) << std::endl;
    std::cout << df.at<std::string>(2u) << std::endl;
    return 0;
}

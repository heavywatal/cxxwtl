#include <wtl/dataframe.hpp>
#include <iostream>

int main() {
    wtl::DataFrame df;
    df.add_column<int>("id");
    df.add_column<double>("value");
    df.add_column<std::string>("label");
    df.reserve(10u);
    df.append(1, 0.1, std::string("a"));
    df.append(2, 0.2, std::string("b"));
    df.append(3, 0.3, std::string("c"));
    std::cout << df;
    return 0;
}

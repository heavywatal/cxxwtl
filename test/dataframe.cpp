#include <wtl/dataframe.hpp>
#include <wtl/iostr.hpp>

int main() {
    wtl::DataFrame df;
    df.reserve_cols(3u);
    df.init_column<int>("id")
      .init_column<double>("value")
      .init_column<std::string>("label");
    df.reserve_rows(3u);
    df.add_row(1, 0.1, std::string("a"));
    df.add_row(2, 0.2, std::string("b"));
    df.add_row(3, 0.3, std::string("c"));
    std::cout << df;
    std::cout << df.colnames() << "\n";
    std::cout << df.at<int>(0u) << "\n";
    std::cout << df.at<double>(1u) << "\n";
    std::cout << df.at<std::string>(2u) << "\n";
    return 0;
}

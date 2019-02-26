#include <wtl/dataframe.hpp>
#include <wtl/iostr.hpp>

int main() {
    wtl::DataFrame df;
    df.reserve_cols(3u);
    df.init_column<int>("id")
      .init_column<double>("value")
      .init_column<std::string>("label");
    df.reserve_rows(4u);
    const int cli = 1;
    const double cld = 1.0;
    const std::string cls = "one";
    int li = 2;
    double ld = 2.0;
    std::string ls = "two";
    df.add_row(0, 0.0, std::string("zero"));
    df.add_row(cli, cld, cls);
    df.add_row(li, ld, ls);
    df.add_row(std::move(li), std::move(ld), std::move(ls));
    std::cout << df;
    std::cout << df.colnames() << "\n";
    std::cout << df.at<int>(0u) << "\n";
    std::cout << df.at<double>(1u) << "\n";
    std::cout << df.at<std::string>(2u) << "\n";
    return 0;
}

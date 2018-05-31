#include <cluster.hpp>
#include <random.hpp>
#include <iostr.hpp>

template <class T> inline
void write(std::ostream& ost, const std::vector<std::valarray<T>>& points, const std::vector<size_t>& labels) {
    ost << "x\ty\tcluster\n";
    for (size_t i=0; i<points.size(); ++i) {
        wtl::join(points[i], ost, "\t") << "\t" << labels[i] << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::cout.precision(4);
    std::vector<std::string> arguments(argv + 1, argv + argc);
    const size_t n = (arguments.size() > 0u) ? std::stoul(arguments[0u]) : 20;
    const size_t k = (arguments.size() > 1u) ? std::stoul(arguments[1u]) : 3;
    std::uniform_real_distribution<double> unif(-1.0, 1.0);
    std::vector<std::valarray<double>> points;
    points.reserve(n);
    for (size_t i=0; i<n; ++i) {
        points.emplace_back(std::initializer_list<double>{unif(wtl::mt64()), unif(wtl::mt64())});
    }
    auto cl = wtl::cluster::pam(std::move(points), k);
    write(std::cout, cl.points(), cl.labels());
    return 0;
}
/* R
library(tidyverse)
df = system2(c("test/test-cluster", 1000, 5), stdout=TRUE) %>%
  paste0(collapse="\n") %>%
  readr::read_tsv()
ggplot(df, aes(x, y)) +
  geom_point(aes(colour = as.factor(cluster)), alpha=0.5) +
  scale_colour_brewer(palette = "Set1", guide = FALSE)
*/

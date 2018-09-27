#include <wtl/random.hpp>
#include <wtl/exception.hpp>

#include <iostream>
#include <fstream>

inline void write_negative_binom(const unsigned n, const double mu, const double k, std::ostream& ost) {
    const double prob = k / (mu + k);
    wtl::negative_binomial_distribution<int> dist(k, prob);
    for (unsigned i=0; i<n; ++i) {
        ost << mu << "\t" << k << "\t" << dist(wtl::mt64()) << "\n";
    }
}

inline void negative_binomial() {
    const unsigned n = 1000u;
    std::ofstream ofs("nbinom.tsv");
    ofs << "mu\tk\tx\n";
    for (const double mu: {1.0, 10.0, 100.0, 1000.0}) {
        for (const double k: {1.0, 10.0, 100.0, 1000.0}) {
            write_negative_binom(n, mu, k, ofs);
        }
    }
}
/* R
df = read_tsv('test/nbinom.tsv')

ggplot(df, aes(x)) +
geom_histogram(bins=50L) +
facet_grid(k ~ mu, scale = "free_x", labeller=label_both)

df %>%
  dplyr::group_by(mu, k) %>%
  dplyr::summarise(mean = mean(x), var = var(x)) %>%
  dplyr::mutate(p = k / (mu + k), E_var = k * (1 - p) / (p ** 2))
*/

inline void canonical() {
    for (size_t i=0; i<6u; ++i) {
        double x = wtl::generate_canonical(wtl::mt64());
        std::cout << x << " ";
        WTL_ASSERT(0.0 <= x && x < 1.0);
    }
    std::cout << "\n";
    wtl::bits64_t min_0x3ff{uint64_t{0x3ff0'0000'0000'0000u}}; // 1.0
    wtl::bits64_t max_0x3ff{uint64_t{0x3fff'ffff'ffff'ffffu}}; // 1.999...
    WTL_ASSERT((min_0x3ff.as_double - 1.0) == 0.0);
    WTL_ASSERT((max_0x3ff.as_double - 1.0) < 1.0);
    std::cout.precision(15);
    std::cout << std::scientific
              << min_0x3ff.as_double - 1.0 << "\n"
              << max_0x3ff.as_double - 1.0 << "\n";
}

int main() {
    negative_binomial();
    canonical();
}

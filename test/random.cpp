#include <wtl/random.hpp>
#include <wtl/exception.hpp>
#include <wtl/iostr.hpp>

#include <iostream>
#include <limits>
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
df = readr::read_tsv('test/nbinom.tsv')

ggplot(df) + aes(x) +
  geom_histogram(bins=50L) +
  facet_grid(vars(k), vars(mu), scale = "free_x", labeller=label_both)

df |>
  dplyr::group_by(mu, k) |>
  dplyr::summarise(mean = mean(x), var = var(x)) |>
  dplyr::mutate(p = k / (mu + k), E_var = k * (1 - p) / (p ** 2))
*/

inline void test_multinomial() {
    wtl::multinomial_distribution multinomial({0.1, 0.2, 0.3});
    WTL_ASSERT(std::abs(multinomial.probabilities()[2] - 0.5) < 1e-9);
    std::cout << multinomial.probabilities() << "\n";
    std::cout << multinomial(wtl::mt64(), 100) << "\n";
}

inline void canonical() {
    constexpr auto epsilon = std::numeric_limits<double>::epsilon();
    constexpr auto max_uint32 = std::numeric_limits<uint32_t>::max();
    constexpr auto max_uint64 = std::numeric_limits<uint64_t>::max();
    static_assert(wtl::detail::as_uint64(max_uint32, max_uint32) == max_uint64);
    static_assert(wtl::detail::as_uint64(1u, 0u) == (uint64_t{1u} << 32));
    static_assert(wtl::detail::as_canonical(0u) == 0.0);
    static_assert(wtl::detail::as_canonical(1u) > epsilon);
    static_assert(wtl::detail::as_canonical(max_uint64) < 1.0);
    static_assert(wtl::detail::as_canonical(max_uint64) == 1.0 - epsilon);
    for (size_t i=0; i<6u; ++i) {
        double x = wtl::generate_canonical(wtl::mt64());
        std::cout << x << " ";
        WTL_ASSERT(0.0 <= x && x < 1.0);
    }
    std::cout << "\n";
}

int main() {
    negative_binomial();
    test_multinomial();
    canonical();
}

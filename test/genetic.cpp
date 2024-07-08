#include <wtl/genetic.hpp>
#include <wtl/random.hpp>
#include <wtl/exception.hpp>

#include <iostream>

int main() {
    constexpr size_t num_generations = 8u;
    constexpr size_t pop_size = 10'000u;
    std::vector<double> fitnesses(pop_size);
    std::vector<double> children(pop_size);
    for (double& w: fitnesses) {
        w = wtl::generate_canonical(wtl::mt64());
    }
    const double mean_fitness_start = wtl::mean(fitnesses);
    std::cout << mean_fitness_start << std::endl;
    for (size_t i=0u; i<num_generations; ++i) {
        auto indices = wtl::roulette_select(fitnesses, pop_size, wtl::mt64());
        children.clear();
        for (const auto j: indices) {
            children.push_back(fitnesses[j]);
        }
        fitnesses.swap(children);
        std::cout << wtl::mean(fitnesses) << std::endl;
    }
    WTL_ASSERT(wtl::mean(fitnesses) > mean_fitness_start);
    return 0;
}

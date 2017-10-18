#pragma once
#ifndef WTL_GENETIC_HPP_
#define WTL_GENETIC_HPP_

#include "numeric.hpp"
#include "algorithm.hpp"

#include <vector>
#include <random>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class RNG> inline
size_t roulette_select(const std::vector<double>& fitnesses, RNG& rng) {
    const std::vector<double> ubounds = partial_sum(fitnesses);
    std::uniform_real_distribution<double> uniform(0.0, ubounds.back());
    return bisect(ubounds, uniform(rng));
}

template <class RNG> inline
std::vector<size_t> roulette_select(
    const std::vector<double>& fitnesses, const size_t n, RNG& rng) {

    const std::vector<double> ubounds = partial_sum(fitnesses);
    std::uniform_real_distribution<double> uniform(0.0, ubounds.back());
    std::vector<size_t> indices;
    indices.reserve(n);
    for (size_t i=0; i<n; ++i) {
        indices.push_back(bisect(ubounds, uniform(rng)));
    }
    return indices;
}

template <class RNG> inline
std::vector<size_t> roulette_select(
    const std::vector<double>& fitnesses,
    RNG& rng,
    const size_t pop_size,
    const size_t elites=0) {

    const std::vector<double> ubounds = partial_sum(fitnesses);

    std::vector<size_t> candidates(pop_size);
    std::iota(begin(candidates), end(candidates), 0);
    std::vector<size_t> children;
    children.reserve(pop_size * 1.1);

    // elite selection
    if (elites) {
        const auto fittest = std::max_element(fitnesses.begin(), fitnesses.end());
        children.assign(elites, fittest - fitnesses.begin());
    }

    // poisson selection
    const double num_fittest_children = pop_size / ubounds.back();
    for (size_t i=0; i<pop_size; ++i) {
        double expected = fitnesses[i];
        expected *= num_fittest_children;
        children.insert(children.end(),
                        std::poisson_distribution<size_t>(expected)(rng), i);
    }

    // roulette selection (slow)
    if (children.size() > pop_size) {
        std::shuffle(std::begin(children), std::end(children), rng);
        children.resize(pop_size);
    }
    else {
        std::uniform_real_distribution<double> uniform(0.0, ubounds.back());
        while (children.size() < pop_size) {
            children.push_back(bisect(ubounds, uniform(rng)));
        }
    }
    return children;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_GENETIC_HPP_ */

#pragma once
#ifndef WTL_GENETIC_HPP_
#define WTL_GENETIC_HPP_

#include <algorithm>
#include <vector>
#include <random>

namespace wtl {

template <class RNG> [[deprecated("use std::discrete_distribution")]] inline
size_t roulette_select(const std::vector<double>& fitnesses, RNG& rng) {
    std::discrete_distribution<size_t> dist(fitnesses.begin(), fitnesses.end());
    return dist(rng);
}

template <class RNG> inline
std::vector<size_t> roulette_select(
    const std::vector<double>& fitnesses, size_t n, RNG& rng) {

    std::discrete_distribution<size_t> dist(fitnesses.begin(), fitnesses.end());
    std::vector<size_t> indices;
    indices.reserve(n);
    for (size_t i=0; i<n; ++i) {
        indices.push_back(dist(rng));
    }
    return indices;
}

template <class RNG> [[deprecated("use std::discrete_distribution")]] inline
std::vector<size_t> roulette_select_cxx11(
    const std::vector<double>& fitnesses, size_t n, RNG& rng) {
    return roulette_select(fitnesses, n, rng);
}

template <class RNG> [[deprecated("not very common")]] inline
std::vector<size_t> roulette_select(
    const std::vector<double>& fitnesses,
    RNG& rng,
    size_t pop_size,
    size_t elites=0) {

    std::vector<size_t> children;
    children.reserve(pop_size);

    elites = std::min(elites, pop_size);
    if (elites) {
        const auto fittest = std::max_element(fitnesses.begin(), fitnesses.end());
        children.assign(elites, static_cast<size_t>(fittest - fitnesses.begin()));
    }
    std::discrete_distribution<size_t> dist(fitnesses.begin(), fitnesses.end());
    for (auto i = children.size(); i < pop_size; ++i) {
        children.push_back(dist(rng));
    }
    return children;
}

} // namespace wtl

#endif /* WTL_GENETIC_HPP_ */

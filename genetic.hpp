// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_GENETIC_HPP_
#define WTL_GENETIC_HPP_

#include <vector>
#include <algorithm>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

//! Make upper bounds from fitness vector
inline std::vector<double> make_ubounds(std::vector<double> v) {
    std::partial_sum(v.begin(), v.end(), v.begin());
    return v;
}

inline size_t throw_dart(const std::vector<double>& ubounds, const double dart) {
    size_t idx = 0;
    while (ubounds[idx] < dart) {++idx;}
    return idx;
}

template <class RNG> inline
size_t roulette_select(const std::vector<double>& fitnesses, RNG& rng) {
    const std::vector<double> ubounds = make_ubounds(fitnesses);
    std::uniform_real_distribution<double> uniform(0.0, ubounds.back());
    return throw_dart(make_ubounds(fitnesses), uniform(rng));
}

template <class RNG> inline
std::vector<size_t> roulette_select(
    const std::vector<double>& fitnesses, const size_t n, RNG& rng) {

    const std::vector<double> ubounds = make_ubounds(fitnesses);
    std::uniform_real_distribution<double> uniform(0.0, ubounds.back());
    std::vector<size_t> selected_idx;
    selected_idx.reserve(n);
    for (size_t i=0; i<n; ++i) {
        selected_idx.push_back(throw_dart(ubounds, uniform(rng)));
    }
    return selected_idx;
}

template <class RNG> inline
std::vector<size_t> roulette_select(
    const std::vector<double>& fitnesses,
    RNG& rng,
    const size_t pop_size,
    const size_t elites=0) {

    const std::vector<double> upper_bounds = make_ubounds(fitnesses);
    const double sum_fitness = upper_bounds.back();

    std::vector<size_t> candidates(pop_size);
    std::iota(begin(candidates), end(candidates), 0);
    std::vector<size_t> children;
    children.reserve(pop_size * 1.1);

    // elite selection
    if (elites) {
        const size_t i = std::distance(
                begin(fitnesses),
                std::max_element(begin(fitnesses), end(fitnesses))
                );
        children.assign(elites, i);
    }

    // poisson selection
    const double num_fittest_children = pop_size / sum_fitness;
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
        std::uniform_real_distribution<double> uniform(0.0, sum_fitness);
        while (children.size() < pop_size) {
            children.push_back(throw_dart(upper_bounds, uniform(rng)));
        }
    }
    return children;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_GENETIC_HPP_ */

// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef GENETIC_HPP_
#define GENETIC_HPP_

#include <vector>
#include <algorithm>

#include "prandom.hpp"

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline std::vector<size_t>
roulette_select(const std::vector<double>& fitnesses, const size_t elites=0) {
    // make a roulette
    const size_t pop_size = fitnesses.size();
    std::vector<double> upper_bounds(pop_size);
    std::partial_sum(begin(fitnesses), end(fitnesses), begin(upper_bounds));
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
        children.insert(children.end(), prandom().poisson(expected), i);
    }

    // roulette selection (slow)
    if (children.size() > pop_size) {
        prandom().shuffle(&children);
        children.resize(pop_size);
    }
    else {
        while (children.size() < pop_size) {
            size_t i = 0;
            const double dart(prandom().uniform(sum_fitness));
            while (upper_bounds[i] < dart) {++i;}
            children.push_back(i);
        }
    }
    return children;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* GENETIC_HPP_ */

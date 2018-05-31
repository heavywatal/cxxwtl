#pragma once
#ifndef WTL_CLUSTER_HPP_
#define WTL_CLUSTER_HPP_

#include "random.hpp"

#include <valarray>
#include <vector>
#include <limits>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

namespace detail {

template <class ValArray>
double euclidean_distance(ValArray&& v) {
    return std::sqrt((v * v).sum());
}

template <class T> inline
size_t classify(
    const std::valarray<T>& x,
    const std::vector<const std::valarray<T>*>& medoids) {

    double min_dist = std::numeric_limits<double>::max();
    size_t best_cluster = std::numeric_limits<size_t>::max();
    for (size_t cluster=0; cluster<medoids.size(); ++cluster) {
        const auto dist = euclidean_distance(x - *medoids[cluster]);
        if (dist < min_dist) {
            min_dist = dist;
            best_cluster = cluster;
        }
    }
    return best_cluster;
}

template <class T> inline
const std::valarray<T>* select_medoid(
    const std::vector<std::valarray<T>>& points,
    const std::vector<size_t>& labels,
    size_t cluster) {

    const size_t n = points.size();
    double min_dist = std::numeric_limits<double>::max();
    size_t medoid_idx = std::numeric_limits<size_t>::max();
    for (size_t i=0; i<n; ++i) {
        if (labels[i] != cluster) continue;
        double dist = 0.0;
        for (size_t j=0; j<n; ++j) {
            if (labels[j] != cluster) continue;
            dist += euclidean_distance(points[i] - points[j]);
        }
        if (dist < min_dist) {
            min_dist = dist;
            medoid_idx = i;
        }
    }
    return &points[medoid_idx];
}

/////////1/////////2/////////3/////////4/////////5/////////6
} // namespace detail
/////////1/////////2/////////3/////////4/////////5/////////6

template <class T, class URBG> inline
std::vector<size_t> k_medoids_pam(
    const std::vector<std::valarray<T>>& points,
    size_t k,
    URBG&& generator,
    size_t max_iteration=10ul) {

    const size_t n = points.size();
    const auto indices = wtl::sample(n, k, generator);
    std::vector<const std::valarray<T>*> medoids;
    medoids.reserve(k);
    for (const auto i: indices) {
        medoids.emplace_back(&points[i]);
    }
    auto prev_medoids = medoids;
    std::vector<size_t> labels(n);
    for (size_t step=0; step<max_iteration; ++step) {
        for (size_t i=0; i<n; ++i) {
            labels[i] = detail::classify(points[i], medoids);
        }
        prev_medoids.swap(medoids);
        for (size_t cluster=0; cluster<k; ++cluster) {
            medoids[cluster] = detail::select_medoid(points, labels, cluster);
        }
        if (prev_medoids == medoids) break;
    }
    return labels;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_CLUSTER_HPP_

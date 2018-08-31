#pragma once
#ifndef WTL_CLUSTER_HPP_
#define WTL_CLUSTER_HPP_

#include "random.hpp"

#include <cmath>
#include <vector>
#include <limits>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace cluster {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class Array>
double euclidean_distance(const Array& lhs, const Array& rhs) {
    using T = typename Array::value_type;
    T sum_squared{0};
    for (size_t i=0; i<lhs.size(); ++i) {
        T d = lhs[i] - rhs[i];
        sum_squared += d * d;
    }
    return std::sqrt(sum_squared);
}

template <class T, class URBG>
class PAM {
  public:
    PAM(const std::vector<T>& points, size_t k, URBG&& engine, size_t max_iteration)
    : points_(points), labels_(points.size()) {
        const auto n = points_.size();
        const auto indices = wtl::sample(points.size(), k, engine);
        medoids_.assign(indices.begin(), indices.end());
        for (size_t step=0; step<max_iteration; ++step) {
            for (size_t i=0; i<n; ++i) {
                labels_[i] = classify(i);
            }
            auto prev_medoids = medoids_;
            for (size_t i=0; i<k; ++i) {
                medoids_[i] = select_medoid(i);
            }
            if (medoids_ == prev_medoids) break;
        }
    }

    const auto& points() const noexcept {return points_;}
    const auto& labels() const noexcept {return labels_;}
    const auto& medoids() const noexcept {return medoids_;}

  private:
    size_t classify(size_t i) const {
        const auto& x = points_[i];
        double min_dist = std::numeric_limits<double>::max();
        size_t best_cluster = std::numeric_limits<size_t>::max();
        for (size_t cluster=0; cluster<medoids_.size(); ++cluster) {
            const auto dist = euclidean_distance(x, points_[medoids_[cluster]]);
            if (dist < min_dist) {
                min_dist = dist;
                best_cluster = cluster;
            }
        }
        return best_cluster;
    }

    size_t select_medoid(size_t cluster) const {
        const size_t n = points_.size();
        double min_dist = std::numeric_limits<double>::max();
        size_t medoid_idx = std::numeric_limits<size_t>::max();
        for (size_t i=0; i<n; ++i) {
            if (labels_[i] != cluster) continue;
            double dist = 0.0;
            for (size_t j=0; j<n; ++j) {
                if (labels_[j] != cluster) continue;
                dist += euclidean_distance(points_[i], points_[j]);
            }
            if (dist < min_dist) {
                min_dist = dist;
                medoid_idx = i;
            }
        }
        return medoid_idx;
    }

    const std::vector<T>& points_;
    std::vector<size_t> labels_;
    std::vector<size_t> medoids_;
};

template <class T, class URBG> inline
auto pam(const T& points, size_t k, URBG&& engine, size_t max_iteration=10ul) {
    return PAM<typename T::value_type, URBG>(points, k, std::forward<URBG>(engine), max_iteration);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::cluster
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_CLUSTER_HPP_

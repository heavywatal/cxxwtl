#pragma once
#ifndef WTL_CLUSTER_HPP_
#define WTL_CLUSTER_HPP_

#include "random.hpp"
#include "signed.hpp"

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
    const auto n = lhs.size();
    for (auto i = decltype(n){}; i < n; ++i) {
        T d = lhs[i] - rhs[i];
        sum_squared += d * d;
    }
    return std::sqrt(sum_squared);
}

template <class T, class URBG>
class PAM {
  public:
    PAM(const std::vector<T>& points, ptrdiff_t k, URBG&& engine, const int max_iteration)
    : points_(points), labels_(points.size()) {
        const auto n = ssize(points);
        const auto indices = wtl::sample(n, k, engine);
        medoids_.assign(indices.begin(), indices.end());
        for (auto step = decltype(max_iteration){}; step < max_iteration; ++step) {
            for (ptrdiff_t i=0; i<n; ++i) {
                at(labels_, i) = classify(i);
            }
            auto prev_medoids = medoids_;
            for (ptrdiff_t i=0; i<k; ++i) {
                at(medoids_, i) = select_medoid(i);
            }
            if (medoids_ == prev_medoids) break;
        }
    }

    const auto& points() const noexcept {return points_;}
    const auto& labels() const noexcept {return labels_;}
    const auto& medoids() const noexcept {return medoids_;}

  private:
    ptrdiff_t classify(ptrdiff_t i) const {
        const auto& x = at(points_, i);
        double min_dist = std::numeric_limits<double>::max();
        ptrdiff_t best_cluster = -1;
        const auto n = ssize(medoids_);
        for (ptrdiff_t cluster=0; cluster<n; ++cluster) {
            const auto dist = euclidean_distance(x, at(points_, at(medoids_, cluster)));
            if (dist < min_dist) {
                min_dist = dist;
                best_cluster = cluster;
            }
        }
        return best_cluster;
    }

    ptrdiff_t select_medoid(ptrdiff_t cluster) const {
        const auto n = ssize(points_);
        double min_dist = std::numeric_limits<double>::max();
        ptrdiff_t medoid_idx = -1;
        for (ptrdiff_t i=0; i<n; ++i) {
            if (at(labels_, i) != cluster) continue;
            double dist = 0.0;
            for (ptrdiff_t j=0; j<n; ++j) {
                if (at(labels_, j) != cluster) continue;
                dist += euclidean_distance(at(points_, i), at(points_, j));
            }
            if (dist < min_dist) {
                min_dist = dist;
                medoid_idx = i;
            }
        }
        return medoid_idx;
    }

    const std::vector<T>& points_;
    std::vector<ptrdiff_t> labels_;
    std::vector<ptrdiff_t> medoids_;
};

template <class T, class URBG> inline
auto pam(const T& points, ptrdiff_t k, URBG&& engine, int max_iteration=10) {
    return PAM<typename T::value_type, URBG>(points, k, std::forward<URBG>(engine), max_iteration);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::cluster
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_CLUSTER_HPP_

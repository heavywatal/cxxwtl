#pragma once
#ifndef WTL_EIGEN_HPP_
#define WTL_EIGEN_HPP_

#include <cstdint>
#include <vector>
#include <valarray>
#include <string>
#include <istream>
#include <Eigen/Core>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl { namespace eigen {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class T> inline
std::vector<typename T::value_type> vector(const T& vec) {
    return std::vector<typename T::value_type>(vec.data(), vec.data() + vec.size());
}

template <class T> inline
std::valarray<typename T::value_type> valarray(const T& vec) {
    return std::valarray<typename T::value_type>(vec.data(), vec.size());
}

template <class T> inline
Eigen::Array<typename T::value_type, Eigen::Dynamic, 1> ArrayX(const T& vec) {
    return Eigen::Array<typename T::value_type, Eigen::Dynamic, 1>::Map(vec.data(), vec.size());
}

template <class T> inline
Eigen::Matrix<typename T::value_type, Eigen::Dynamic, 1> VectorX(const T& vec) {
    return Eigen::Matrix<typename T::value_type, Eigen::Dynamic, 1>::Map(vec.data(), vec.size());
}

template <class T> inline
Eigen::Matrix<typename T::value_type, 1, Eigen::Dynamic> RowVectorX(const T& vec) {
    return Eigen::Matrix<typename T::value_type, 1, Eigen::Dynamic>::Map(vec.data(), vec.size());
}

template <class T> inline
Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
ArrayXX(const std::vector<std::string>& rows) {
    const size_t nrow = rows.size();
    const size_t ncol = rows[0].length();
    Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> array(nrow, ncol);
    for (size_t i=0; i<nrow; ++i) {
        const std::string& s = rows[i];
        const auto bytes = std::vector<uint8_t>(s.begin(), s.end());
        array.row(i) = RowVectorX(bytes).cast<T>();
    }
    // ASCII: char(0, 1) <=> int(48, 49)
    return array -= 48;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

template <class Vector> inline
std::vector<size_t> which(const Vector& predicate) {
    std::vector<size_t> indices;
    const size_t n = predicate.size();
    indices.reserve(n);
    for (size_t i=0; i<n; ++i) {
        if (predicate[i] > 0) {indices.push_back(i);}
    }
    return indices;
}

template <class T, class Vector> inline
T slice(const Eigen::DenseBase<T>& orig, const Vector& indices) {
    const size_t n = indices.size();
    T result(n, orig.cols());
    for (size_t i=0; i<n; ++i) {
        result.row(i) = orig.row(indices[i]);
    }
    return result;
}

template <class T, class Vector> inline
T slice_cols(const Eigen::DenseBase<T>& orig, const Vector& indices) {
    const size_t n = indices.size();
    T result(orig.rows(), n);
    for (size_t i=0; i<n; ++i) {
        result.col(i) = orig.col(indices[i]);
    }
    return result;
}

template <class T, class Vector> inline
T filter(const Eigen::DenseBase<T>& orig, const Vector& predicate) {
    return slice(orig, which(predicate));
}

template <class T, class Vector> inline
T select(const Eigen::DenseBase<T>& orig, const Vector& predicate) {
    return slice_cols(orig, which(predicate));
}

inline Eigen::IOFormat tsv(const int precision=Eigen::StreamPrecision, const std::string& sep="\t") {
    return {precision, Eigen::DontAlignCols, sep, "\n"};
}

template <class T> inline
std::vector<Eigen::Array<typename T::value_type, Eigen::Dynamic, 1, Eigen::ColMajor>>
columns(const Eigen::DenseBase<T>& matrix) {
    const size_t n = matrix.cols();
    std::vector<Eigen::Array<typename T::value_type, Eigen::Dynamic, 1, Eigen::ColMajor>> result;
    result.reserve(n);
    for (size_t i=0; i<n; ++i) {
        result.push_back(matrix.col(i));
    }
    return result;
}

template <class T> inline
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
read_matrix(std::istream& fin, size_t ncol) {
    std::vector<T> vec{std::istream_iterator<T>(fin), std::istream_iterator<T>()};
    if (vec.size() % ncol > 0) {
        std::string msg = std::string(__PRETTY_FUNCTION__);
        throw std::runtime_error(msg + ": vec.size() % ncol > 0");
    }
    return Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>::Map(vec.data(), vec.size() / ncol, ncol);
}

template <class T> inline
Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
read_array(std::istream& fin, size_t ncol) {
    std::vector<T> vec{std::istream_iterator<T>(fin), std::istream_iterator<T>()};
    if (vec.size() % ncol > 0) {
        std::string msg = std::string(__PRETTY_FUNCTION__);
        throw std::runtime_error(msg + ": vec.size() % ncol > 0");
    }
    return Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>::Map(vec.data(), vec.size() / ncol, ncol);
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
}} // namespace wtl::eigen
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_EIGEN_HPP_

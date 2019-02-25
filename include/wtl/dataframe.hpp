#pragma once
#ifndef WTL_DATAFRAME_HPP_
#define WTL_DATAFRAME_HPP_

#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <ostream>

namespace wtl {

namespace detail {

class ColumnBase {
  public:
    ColumnBase() = default;
    virtual ~ColumnBase() = default;
    virtual void reserve(size_t n) = 0;
    virtual std::ostream& write_at(std::ostream& ost, size_t i) const = 0;
  private:
};

template <class T>
class Column: public ColumnBase {
  public:
    Column() = default;
    ~Column() = default;
    void push_back(T&& x) {
        vec_.push_back(std::forward<T>(x));
    }
    void reserve(size_t n) override {
        vec_.reserve(n);
    }
    std::ostream& write_at(std::ostream& ost, size_t i) const override {
        return ost << vec_[i];
    }
  private:
    std::vector<T> vec_;
};

} // namespace detail

class DataFrame {
  public:
    template <class T>
    void add_column(const std::string& name) {
        if (nrow_ > 0u) {
            throw std::runtime_error("cannot add a column after append()");
        }
        colnames_.push_back(name);
        table_.emplace_back(std::make_unique<detail::Column<T>>());
    }
    void reserve(size_t n) {
        for (const auto& column: table_) {
            column->reserve(n);
        }
    }
    template <class... Args>
    void append(Args&&... args) {
        append_impl(0, std::forward<Args>(args)...);
        ++nrow_;
    }
    std::ostream& write(std::ostream& ost, const char* sep = "\t") const {
        write_header(ost, sep);
        for (size_t i = 0; i < nrow_; ++i) {
            write_at(ost, i, sep);
        }
        return ost;
    }
    friend std::ostream& operator<<(std::ostream &ost, const DataFrame& x) {
        return x.write(ost);
    }
    size_t nrow() const noexcept {return nrow_;}
    size_t ncol() const noexcept {return colnames_.size();}
  private:
    template <class T, class... Args>
    void append_impl(size_t i, T&& x, Args&&... args) {
        append_impl(i, std::forward<T>(x));
        append_impl(++i, std::forward<Args>(args)...);
    }
    template <class T>
    void append_impl(size_t i, T&& x) {
        auto p = dynamic_cast<detail::Column<T>*>(table_[i].get());
        p->push_back(std::forward<T>(x));
    }
    std::ostream& write_header(std::ostream& ost, const char* sep = "\t") const {
        if (colnames_.empty()) return ost;
        auto it = colnames_.begin();
        ost << *it;
        while (++it != colnames_.end()) {
            ost << sep << *it;
        }
        ost << "\n";
        return ost;
    }
    std::ostream& write_at(std::ostream& ost, size_t i, const char* sep = "\t") const {
        if (table_.empty()) return ost;
        auto it = table_.begin();
        (*it)->write_at(ost, i);
        while (++it != table_.end()) {
            (*it)->write_at(ost << sep, i);
        }
        ost << "\n";
        return ost;
    }
    std::vector<std::unique_ptr<detail::ColumnBase>> table_;
    std::vector<std::string> colnames_;
    size_t nrow_ = 0u;
};

} // namespace wtl

#endif // WTL_DATAFRAME_HPP_

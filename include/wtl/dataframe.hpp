#pragma once
#ifndef WTL_DATAFRAME_HPP_
#define WTL_DATAFRAME_HPP_

#include "signed.hpp"

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
    virtual void reserve(ptrdiff_t n) = 0;
    virtual std::ostream& write_at(std::ostream& ost, ptrdiff_t i) const = 0;
  private:
};

template <class T>
class Column: public ColumnBase {
  public:
    Column() = default;
    ~Column() = default;
    void reserve(ptrdiff_t n) override {
        wtl::reserve(data_, n);
    }
    std::ostream& write_at(std::ostream& ost, ptrdiff_t i) const override {
        return ost << at(data_, i);
    }
    std::vector<T>& data() noexcept {return data_;}
  private:
    std::vector<T> data_;
};

} // namespace detail

class DataFrame {
  public:
    DataFrame() = default;
    DataFrame(const DataFrame&) = default;
    DataFrame(DataFrame&&) = default;
    ~DataFrame() noexcept = default;

    template <class T>
    DataFrame& init_column(std::string name) {
        if (nrow_ > 0u) {
            throw std::logic_error("cannot add columns after add_row()");
        }
        colnames_.emplace_back(std::move(name));
        columns_.emplace_back(std::make_unique<detail::Column<T>>());
        return *this;
    }
    DataFrame& reserve_cols(ptrdiff_t n) {
        wtl::reserve(columns_, n);
        return *this;
    }
    DataFrame& reserve_rows(ptrdiff_t n) {
        for (const auto& column: columns_) {
            column->reserve(n);
        }
        return *this;
    }
    template <class... Args>
    DataFrame& add_row(Args&&... args) {
        add_row_impl(0, std::forward<Args>(args)...);
        ++nrow_;
        return *this;
    }
    std::ostream& write(std::ostream& ost, const char* sep = "\t") const {
        write_header(ost, sep);
        for (ptrdiff_t i = 0; i < nrow_; ++i) {
            write_row(ost, i, sep);
        }
        return ost;
    }
    friend std::ostream& operator<<(std::ostream &ost, const DataFrame& x) {
        return x.write(ost);
    }
    template <class T>
    const std::vector<T>& at(ptrdiff_t i) const {
        return dynamic_cast<detail::Column<T>*>(wtl::at(columns_, i).get())->data();
    }
    const std::vector<std::string> colnames() const noexcept {return colnames_;}
    ptrdiff_t ncol() const noexcept {return ssize(columns_);}
    ptrdiff_t nrow() const noexcept {return nrow_;}

  private:
    template <class T, class... Rest>
    void add_row_impl(ptrdiff_t i, T&& x, Rest&&... rest) {
        add_row_impl(i, std::forward<T>(x));
        add_row_impl(++i, std::forward<Rest>(rest)...);
    }
    template <class T>
    void add_row_impl(ptrdiff_t i, T&& x) {
        using value_type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
        auto col_i = dynamic_cast<detail::Column<value_type>*>(wtl::at(columns_, i).get());
        col_i->data().push_back(std::forward<T>(x));
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
    std::ostream& write_row(std::ostream& ost, ptrdiff_t i, const char* sep = "\t") const {
        if (columns_.empty()) return ost;
        auto it = columns_.begin();
        (*it)->write_at(ost, i);
        while (++it != columns_.end()) {
            (*it)->write_at(ost << sep, i);
        }
        ost << "\n";
        return ost;
    }
    std::vector<std::unique_ptr<detail::ColumnBase>> columns_;
    std::vector<std::string> colnames_;
    ptrdiff_t nrow_ = 0;
};

} // namespace wtl

#endif // WTL_DATAFRAME_HPP_

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
    void reserve(size_t n) override {
        data_.reserve(n);
    }
    std::ostream& write_at(std::ostream& ost, size_t i) const override {
        return ost << data_[i];
    }
    std::vector<T>& data() noexcept {return data_;}
  private:
    std::vector<T> data_;
};

} // namespace detail

class DataFrame {
  public:
    DataFrame() = default;
    DataFrame(const DataFrame&) = delete;
    DataFrame(DataFrame&&) = delete;
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
    DataFrame& reserve_cols(size_t n) {
        columns_.reserve(n);
        return *this;
    }
    DataFrame& reserve_rows(size_t n) {
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
        for (size_t i = 0; i < nrow_; ++i) {
            write_row(ost, i, sep);
        }
        return ost;
    }
    friend std::ostream& operator<<(std::ostream &ost, const DataFrame& x) {
        return x.write(ost);
    }
    template <class T>
    const std::vector<T>& at(size_t i) const {
        return dynamic_cast<detail::Column<T>*>(columns_.at(i).get())->data();
    }
    const std::vector<std::string> colnames() const noexcept {return colnames_;}
    size_t ncol() const noexcept {return columns_.size();}
    size_t nrow() const noexcept {return nrow_;}

  private:
    template <class T, class... Rest>
    void add_row_impl(size_t i, T&& x, Rest&&... rest) {
        add_row_impl(i, std::forward<T>(x));
        add_row_impl(++i, std::forward<Rest>(rest)...);
    }
    template <class T>
    void add_row_impl(size_t i, T&& x) {
        using value_type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
        auto col_i = dynamic_cast<detail::Column<value_type>*>(columns_[i].get());
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
    std::ostream& write_row(std::ostream& ost, size_t i, const char* sep = "\t") const {
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
    size_t nrow_ = 0u;
};

} // namespace wtl

#endif // WTL_DATAFRAME_HPP_

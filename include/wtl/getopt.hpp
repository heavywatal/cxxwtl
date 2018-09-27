#pragma once

#include "iostr.hpp"
#include <boost/program_options.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline std::ostream&
ost_any(std::ostream& ost, const boost::any& value) {
    if (value.type() == typeid(int)) {
        ost << boost::any_cast<int>(value);
    }
    else if (value.type() == typeid(unsigned int)) {
        ost << boost::any_cast<unsigned int>(value);
    }
    else if (value.type() == typeid(ptrdiff_t)) {
        ost << boost::any_cast<ptrdiff_t>(value);
    }
    else if (value.type() == typeid(size_t)) {
        ost << boost::any_cast<size_t>(value);
    }
    else if (value.type() == typeid(double)) {
        ost << boost::any_cast<double>(value);
    }
    else if (value.type() == typeid(bool)) {
        ost << boost::any_cast<bool>(value);
    }
    else if (value.type() == typeid(std::string)) {
        ost << boost::any_cast<std::string>(value);
    }
    else {
        ost << boost::any_cast<unsigned long>(value);
    }
    return ost;
}

inline std::string str_any(const boost::any& value) {
    std::ostringstream oss;
    ost_any(oss, value);
    return oss.str();
}

inline std::ostream&
flags_into_stream(std::ostream& ost,
                  const boost::program_options::variables_map& vm) {
    for (const auto& pair: vm) {
        boost::any value = pair.second.value();
        if (value.type() == typeid(std::string)
            && boost::any_cast<std::string>(value).empty()) {
            ost << '#';
        }
        ost_any(ost << pair.first << " = ", value) << "\n";
    }
    return ost;
}

inline std::string
flags_into_string(const boost::program_options::variables_map& vm) {
    std::ostringstream oss;
    oss.precision(std::numeric_limits<double>::max_digits10);
    flags_into_stream(oss, vm);
    return oss.str();
}

template <class DelimT=const char*>
inline std::ostream&
write_table(const boost::program_options::variables_map& vm,
            std::ostream& ost, DelimT&& delim="\t") {
    wtl::join(vm, ost, delim,
      [](const auto& p){return p.first;}) << "\n";
    wtl::join(vm, ost, delim,
      [](const auto& p){return str_any(p.second.value());}) << "\n";
    return ost;
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

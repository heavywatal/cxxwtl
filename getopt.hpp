// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <ostream>
#include <sstream>
#include <boost/program_options.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline std::ostream&
flags_into_stream(std::ostream& ost,
                  const boost::program_options::variables_map& vm) {
    for (const auto& pair: vm) {
        boost::any value = pair.second.value();
        if (value.type() == typeid(std::string)
            && boost::any_cast<std::string>(value).empty()) {
            ost << '#';
        }
        ost << pair.first << " = ";
        if (value.type() == typeid(int)) {
            ost << boost::any_cast<int>(value);
        }
        else if (value.type() == typeid(unsigned int)) {
            ost << boost::any_cast<unsigned int>(value);
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
            ost << '"' << boost::any_cast<std::string>(value) << '"';
        }
        else {
            ost << boost::any_cast<unsigned long>(value);
        }
        ost << '\n';
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

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

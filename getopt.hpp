// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <sstream>
#include <boost/program_options.hpp>

inline std::string flags_into_string(
            const boost::program_options::options_description& desc,
            const boost::program_options::variables_map& vm) {

    std::ostringstream oss;
    oss.precision(16);
    for (const auto shared_ptr_opt: desc.options()) {
        std::string long_name(shared_ptr_opt.get()->long_name());
        boost::any value(vm[long_name].value());
        if (!vm.count(long_name)) {continue;}
        if (value.type() == typeid(std::string)
            && boost::any_cast<std::string>(value).empty()) {
            oss << '#';
        }
        oss << long_name << " = ";
        if (value.type() == typeid(int)) {
            oss << boost::any_cast<int>(value);
        }
        else if (value.type() == typeid(unsigned int)) {
            oss << boost::any_cast<unsigned int>(value);
        }
        else if (value.type() == typeid(size_t)) {
            oss << boost::any_cast<size_t>(value);
        }
        else if (value.type() == typeid(double)) {
            oss << boost::any_cast<double>(value);
        }
        else if (value.type() == typeid(bool)) {
            oss << boost::any_cast<bool>(value);
        }
        else if (value.type() == typeid(std::string)) {
            oss << boost::any_cast<std::string>(value);
        }
        else {
            oss << boost::any_cast<unsigned long>(value);
        }
        oss << '\n';
    }
    return oss.str();
}

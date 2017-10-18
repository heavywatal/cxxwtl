#pragma once
#ifndef WTL_DEMANGLE_HPP_
#define WTL_DEMANGLE_HPP_

#include <cxxabi.h> // demangle

#include <string>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

inline std::string demangle(const char * type_info_name) {
    int status(0);
    return std::string(abi::__cxa_demangle(type_info_name, 0, 0, &status));
}

template <class T>
inline std::string typestr(const T& x) {
    return demangle(typeid(x).name());
}

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif /* WTL_DEMANGLE_HPP_ */

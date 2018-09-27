#pragma once
#ifndef WTL_DEBUG_HPP_
#define WTL_DEBUG_HPP_

#include <cstdio>
#include <iostream>

#define CERR(...) std::cerr << "\033[1;30m" << __VA_ARGS__ << "\033[0m"

#ifndef NDEBUG
#  define DCERR(...) CERR(__VA_ARGS__)
#  define DPRINTF(...) std::fprintf(stderr, __VA_ARGS__)
#else
#  define DCERR(...)
#  define DPRINTF(...)
#endif // NDEBUG

#define FILE_LINE_PRETTY __FILE__<<':'<<__LINE__<<':'<<__PRETTY_FUNCTION__
#define FLPF CERR(FILE_LINE_PRETTY << std::endl)
#define HERE DCERR(FILE_LINE_PRETTY << std::endl)

#endif // WTL_DEBUG_HPP_

// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef OMP_HPP_
#define OMP_HPP_

#if defined(_OPENMP)
#  include <omp.h>
#  define PRAGMA_(x) _Pragma(#x)
#  define OMP(x) PRAGMA_(omp x)
#  define OMP_BEGIN_PAREN(x) OMP(x) {
#  define OMP_END_PAREN }
#  define IF_OMP(x) x
#else
#  define OMP(x)
#  define OMP_BEGIN_PAREN(x)
#  define OMP_END_PAREN
#  define IF_OMP(x)
#endif

#endif /* OMP_HPP_ */

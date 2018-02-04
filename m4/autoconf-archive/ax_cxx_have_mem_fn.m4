# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_cxx_have_mem_fn.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_MEM_FN()
#
# DESCRIPTION
#
#   This macro checks if std::mem_fn, added in C++11, is defined in the
#   <functional> header.
#
#   If it is, define the ax_cv_cxx_have_mem_fn environment variable to "yes"
#   and define HAVE_CXX_MEM_FN.
#
# LICENSE
#
#   Copyright (c) 2014 Enrico M. Crisostomo <enrico.m.crisostomo@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_CXX_HAVE_MEM_FN],
  [AC_CACHE_CHECK(
    [for std::mem_fn in functional],
    ax_cv_cxx_have_mem_fn,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using namespace std;]
          [struct st { int x; int fn() { return x; } };]
        ],
        [
          [st one {1};]
          [auto mem_fned = mem_fn(&st::fn);]
          [mem_fned(one);]
        ]
        )],
        [ax_cv_cxx_have_mem_fn=yes],
        [ax_cv_cxx_have_mem_fn=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_mem_fn" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_MEM_FN,
        1,
        [Define if functional defines the std::mem_fn class.])
    fi
  ])

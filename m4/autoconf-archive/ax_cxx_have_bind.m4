# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_cxx_have_bind.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_BIND()
#
# DESCRIPTION
#
#   This macro checks if std::bind, added in C++11, is defined in the
#   <functional> header.
#
#   If it is, define the ax_cv_cxx_have_bind environment variable to "yes"
#   and define HAVE_CXX_BIND.
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

AC_DEFUN([AX_CXX_HAVE_BIND],
  [AC_CACHE_CHECK(
    [for std::bind in functional],
    ax_cv_cxx_have_bind,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using namespace std;]
          [int fn(int x, int y) { return x + y; }]
        ],
        [
          [auto bound_fn = bind(fn,1,2);]
          [bound_fn();]
        ]
        )],
        [ax_cv_cxx_have_bind=yes],
        [ax_cv_cxx_have_bind=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_bind" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_BIND,
        1,
        [Define if functional defines the std::bind class.])
    fi
  ])

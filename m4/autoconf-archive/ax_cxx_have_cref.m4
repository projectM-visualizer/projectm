# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_cxx_have_cref.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_CREF()
#
# DESCRIPTION
#
#   This macro checks if std::cref, added in C++11, is defined in the
#   <functional> header.
#
#   If it is, define the ax_cv_cxx_have_cref environment variable to "yes"
#   and define HAVE_CXX_CREF.
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

AC_DEFUN([AX_CXX_HAVE_CREF],
  [AC_CACHE_CHECK(
    [for std::cref in functional],
    ax_cv_cxx_have_cref,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using namespace std;]
        ],
        [
          [int x(1);]
          [auto crefd = cref(x);]
        ]
        )],
        [ax_cv_cxx_have_cref=yes],
        [ax_cv_cxx_have_cref=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_cref" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_CREF,
        1,
        [Define if functional defines the std::cref class.])
    fi
  ])

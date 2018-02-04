# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_have_bit_and.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_BIT_AND()
#
# DESCRIPTION
#
#   This macro checks if std::bit_and, added in C++11, is defined in the
#   <functional> header.
#
#   If it is, define the ax_cv_cxx_have_bit_and environment variable to
#   "yes" and define HAVE_CXX_BIT_AND.
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

AC_DEFUN([AX_CXX_HAVE_BIT_AND],
  [AC_CACHE_CHECK(
    [for std::bit_and in functional],
    ax_cv_cxx_have_bit_and,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::bit_and;]
        ],
        []
        )],
        [ax_cv_cxx_have_bit_and=yes],
        [ax_cv_cxx_have_bit_and=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_bit_and" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_BIT_AND,
        1,
        [Define if functional defines the std::bit_and class.])
    fi
  ])

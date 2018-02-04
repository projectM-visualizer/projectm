# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_have_bit_xor.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_BIT_XOR()
#
# DESCRIPTION
#
#   This macro checks if std::bit_xor, added in C++11, is defined in the
#   <functional> header.
#
#   If it is, define the ax_cv_cxx_have_bit_xor environment variable to
#   "yes" and define HAVE_CXX_BIT_XOR.
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

AC_DEFUN([AX_CXX_HAVE_BIT_XOR],
  [AC_CACHE_CHECK(
    [for std::bit_xor in functional],
    ax_cv_cxx_have_bit_xor,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::bit_xor;]
        ],
        []
        )],
        [ax_cv_cxx_have_bit_xor=yes],
        [ax_cv_cxx_have_bit_xor=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_bit_xor" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_BIT_XOR,
        1,
        [Define if functional defines the std::bit_xor class.])
    fi
  ])

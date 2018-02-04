# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_cxx_have_ref.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_REF()
#
# DESCRIPTION
#
#   This macro checks if std::ref, added in C++11, is defined in the
#   <functional> header.
#
#   If it is, define the ax_cv_cxx_have_ref environment variable to "yes"
#   and define HAVE_CXX_REF.
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

AC_DEFUN([AX_CXX_HAVE_REF],
  [AC_CACHE_CHECK(
    [for std::ref in functional],
    ax_cv_cxx_have_ref,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using namespace std;]
        ],
        [
          [int one(1);]
          [auto refed = ref(one);]
          [++refed;]
        ]
        )],
        [ax_cv_cxx_have_ref=yes],
        [ax_cv_cxx_have_ref=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_ref" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_REF,
        1,
        [Define if functional defines the std::ref class.])
    fi
  ])

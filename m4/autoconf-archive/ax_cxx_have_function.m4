# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_have_function.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_FUNCTION()
#
# DESCRIPTION
#
#   This macro checks if std::function, added in C++11, is defined in the
#   <functional> header.
#
#   If it is, define the ax_cv_cxx_have_function environment variable to
#   "yes" and define HAVE_CXX_FUNCTION.
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

AC_DEFUN([AX_CXX_HAVE_FUNCTION],
  [AC_CACHE_CHECK(
    [for std::bad_function_call in functional],
    ax_cv_cxx_have_function,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::bad_function_call;]
        ],
        []
        )],
        [ax_cv_cxx_have_function=yes],
        [ax_cv_cxx_have_function=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_function" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_FUNCTION,
        1,
        [Define if functional defines the std::bad_function_call class.])
    fi
  ])

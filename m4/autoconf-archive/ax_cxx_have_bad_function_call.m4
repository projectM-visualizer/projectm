# ==================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_bad_function_call.html
# ==================================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_BAD_FUNCTION_CALL()
#
# DESCRIPTION
#
#   This macro checks if std::bad_function_call, added in C++11, is defined
#   in the <functional> header.
#
#   If it is, define the ax_cv_cxx_have_bad_function_call environment
#   variable to "yes" and define HAVE_CXX_BAD_FUNCTION_CALL.
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

AC_DEFUN([AX_CXX_HAVE_BAD_FUNCTION_CALL],
  [AC_CACHE_CHECK(
    [for std::bad_function_call in functional],
    ax_cv_cxx_have_bad_function_call,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::bad_function_call;]
        ],
        []
        )],
        [ax_cv_cxx_have_bad_function_call=yes],
        [ax_cv_cxx_have_bad_function_call=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_bad_function_call" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_BAD_FUNCTION_CALL,
        1,
        [Define if functional defines the std::bad_function_call class.])
    fi
  ])

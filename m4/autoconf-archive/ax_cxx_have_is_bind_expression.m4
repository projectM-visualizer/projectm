# ===================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_is_bind_expression.html
# ===================================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_IS_BIND_EXPRESSION()
#
# DESCRIPTION
#
#   This macro checks if std::is_bind_expression, added in C++11, is defined
#   in the <functional> header.
#
#   If it is, define the ax_cv_cxx_have_is_bind_expression environment
#   variable to "yes" and define HAVE_CXX_IS_BIND_EXPRESSION.
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

AC_DEFUN([AX_CXX_HAVE_IS_BIND_EXPRESSION],
  [AC_CACHE_CHECK(
    [for std::is_bind_expression in functional],
    ax_cv_cxx_have_is_bind_expression,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::is_bind_expression;]
        ],
        []
        )],
        [ax_cv_cxx_have_is_bind_expression=yes],
        [ax_cv_cxx_have_is_bind_expression=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_is_bind_expression" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_IS_BIND_EXPRESSION,
        1,
        [Define if functional defines the std::is_bind_expression class.])
    fi
  ])

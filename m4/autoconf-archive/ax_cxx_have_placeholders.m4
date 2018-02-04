# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_placeholders.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_PLACEHOLDERS()
#
# DESCRIPTION
#
#   This macro checks if std::placeholders, added in C++11, is defined in
#   the <functional> header.
#
#   If it is, define the ax_cv_cxx_have_placeholders environment variable to
#   "yes" and define HAVE_CXX_PLACEHOLDERS.
#
# LICENSE
#
#   Copyright (c) 2014 Enrico M. Crisostomo <enrico.m.crisostomo@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_CXX_HAVE_PLACEHOLDERS],
  [AC_CACHE_CHECK(
    [for std::placeholders in functional],
    ax_cv_cxx_have_placeholders,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using namespace std;]
          [using namespace std::placeholders;]
          [int fn(int x, int y) { return x + y; }]
        ],
        [
          [auto bound_function = bind (fn, 10, _1);]
          [bound_function(20);]
        ]
        )],
        [ax_cv_cxx_have_placeholders=yes],
        [ax_cv_cxx_have_placeholders=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_placeholders" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_PLACEHOLDERS,
        1,
        [Define if functional defines the std::placeholders namespace.])
    fi
  ])

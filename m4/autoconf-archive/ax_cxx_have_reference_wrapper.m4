# ==================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_reference_wrapper.html
# ==================================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_REFERENCE_WRAPPER()
#
# DESCRIPTION
#
#   This macro checks if std::reference_wrapper, added in C++11, is defined
#   in the <functional> header.
#
#   If it is, define the ax_cv_cxx_have_reference_wrapper environment
#   variable to "yes" and define HAVE_CXX_REFERENCE_WRAPPER.
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

AC_DEFUN([AX_CXX_HAVE_REFERENCE_WRAPPER],
  [AC_CACHE_CHECK(
    [for std::reference_wrapper in functional],
    ax_cv_cxx_have_reference_wrapper,
    [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
          [#include <functional>]
          [using std::reference_wrapper;]
        ],
        []
        )],
        [ax_cv_cxx_have_reference_wrapper=yes],
        [ax_cv_cxx_have_reference_wrapper=no]
      )
    AC_LANG_POP([C++])])
    if test x"$ax_cv_cxx_have_reference_wrapper" = "xyes"
    then
      AC_DEFINE(HAVE_CXX_REFERENCE_WRAPPER,
        1,
        [Define if functional defines the std::reference_wrapper class.])
    fi
  ])

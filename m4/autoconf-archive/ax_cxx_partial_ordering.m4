# ============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_partial_ordering.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_CXX_PARTIAL_ORDERING
#
# DESCRIPTION
#
#   If the compiler supports partial ordering, define HAVE_PARTIAL_ORDERING.
#
# LICENSE
#
#   Copyright (c) 2008 Todd Veldhuizen
#   Copyright (c) 2008 Luc Maisonobe <luc@spaceroots.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([AC_CXX_PARTIAL_ORDERING], [AX_CXX_PARTIAL_ORDERING])
AC_DEFUN([AX_CXX_PARTIAL_ORDERING],
[AC_CACHE_CHECK(whether the compiler supports partial ordering,
ax_cv_cxx_partial_ordering,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
template<int N> struct I {};
template<class T> struct A
{  int r;
   template<class T1, class T2> int operator() (T1, T2)       { r = 0; return r; }
   template<int N1, int N2>     int operator() (I<N1>, I<N2>) { r = 1; return r; }
};],[A<float> x, y; I<0> a; I<1> b; return x (a,b) + y (float(), double());],
 ax_cv_cxx_partial_ordering=yes, ax_cv_cxx_partial_ordering=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_partial_ordering" = yes; then
  AC_DEFINE(HAVE_PARTIAL_ORDERING,,
            [define if the compiler supports partial ordering])
fi
])

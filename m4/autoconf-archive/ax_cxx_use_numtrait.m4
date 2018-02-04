# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_use_numtrait.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_USE_NUMTRAIT
#
# DESCRIPTION
#
#   If the compiler supports numeric traits promotions, define
#   HAVE_USE_NUMTRAIT.
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

AU_ALIAS([AC_CXX_USE_NUMTRAIT], [AX_CXX_USE_NUMTRAIT])
AC_DEFUN([AX_CXX_USE_NUMTRAIT],
[AC_CACHE_CHECK(whether the compiler supports numeric traits promotions,
ax_cv_cxx_use_numtrait,
[AC_REQUIRE([AX_CXX_TYPENAME])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
#ifndef HAVE_TYPENAME
 #define typename
#endif
template<class T_numtype> class SumType       { public : typedef T_numtype T_sumtype;   };
template<>                class SumType<char> { public : typedef int T_sumtype; };
template<class T> class A {};
template<class T> A<typename SumType<T>::T_sumtype> sum(A<T>)
{ return A<typename SumType<T>::T_sumtype>(); }
],[A<float> x; sum(x); return 0;],
 ax_cv_cxx_use_numtrait=yes, ax_cv_cxx_use_numtrait=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_use_numtrait" = yes; then
  AC_DEFINE(HAVE_USE_NUMTRAIT,,[define if the compiler supports numeric traits promotions])
fi
])

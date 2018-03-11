# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_have_complex.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_COMPLEX
#
# DESCRIPTION
#
#   If the compiler has complex<T>, define HAVE_COMPLEX.
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

AU_ALIAS([AC_CXX_HAVE_COMPLEX], [AX_CXX_HAVE_COMPLEX])
AC_DEFUN([AX_CXX_HAVE_COMPLEX],
[AC_CACHE_CHECK(whether the compiler has complex<T>,
ax_cv_cxx_have_complex,
[AC_REQUIRE([AX_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <complex>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[complex<float> a; complex<double> b; return 0;],
 ax_cv_cxx_have_complex=yes, ax_cv_cxx_have_complex=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_complex" = yes; then
  AC_DEFINE(HAVE_COMPLEX,,[define if the compiler has complex<T>])
fi
])

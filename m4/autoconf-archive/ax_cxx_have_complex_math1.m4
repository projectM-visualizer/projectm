# ==============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_complex_math1.html
# ==============================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_COMPLEX_MATH1
#
# DESCRIPTION
#
#   If the compiler has the complex math functions cos, cosh, exp, log, pow,
#   sin, sinh, sqrt, tan and tanh, define HAVE_COMPLEX_MATH1.
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

AU_ALIAS([AC_CXX_HAVE_COMPLEX_MATH1], [AX_CXX_HAVE_COMPLEX_MATH1])
AC_DEFUN([AX_CXX_HAVE_COMPLEX_MATH1],
[AC_CACHE_CHECK(whether the compiler has complex math functions,
ax_cv_cxx_have_complex_math1,
[AC_REQUIRE([AX_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 ac_save_LIBS="$LIBS"
 LIBS="$LIBS -lm"
 AC_TRY_LINK([#include <complex>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[complex<double> x(1.0, 1.0), y(1.0, 1.0);
cos(x); cosh(x); exp(x); log(x); pow(x,1); pow(x,double(2.0));
pow(x, y); pow(double(2.0), x); sin(x); sinh(x); sqrt(x); tan(x); tanh(x);
return 0;],
 ax_cv_cxx_have_complex_math1=yes, ax_cv_cxx_have_complex_math1=no)
 LIBS="$ac_save_LIBS"
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_complex_math1" = yes; then
  AC_DEFINE(HAVE_COMPLEX_MATH1,,[define if the compiler has complex math functions])
fi
])

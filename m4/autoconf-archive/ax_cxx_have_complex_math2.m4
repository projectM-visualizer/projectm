# ==============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_complex_math2.html
# ==============================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_COMPLEX_MATH2
#
# DESCRIPTION
#
#   If the compiler has the complex math functions acos, asin, atan, atan2
#   and log10, define HAVE_COMPLEX_MATH2.
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

AU_ALIAS([AC_CXX_HAVE_COMPLEX_MATH2], [AX_CXX_HAVE_COMPLEX_MATH2])
AC_DEFUN([AX_CXX_HAVE_COMPLEX_MATH2],
[AC_CACHE_CHECK(whether the compiler has more complex math functions,
ax_cv_cxx_have_complex_math2,
[AC_REQUIRE([AX_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 ac_save_LIBS="$LIBS"
 LIBS="$LIBS -lm"
 AC_TRY_LINK([#include <complex>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[complex<double> x(1.0, 1.0), y(1.0, 1.0);
acos(x); asin(x); atan(x); atan2(x,y); atan2(x, double(3.0));
atan2(double(3.0), x); log10(x); return 0;],
 ax_cv_cxx_have_complex_math2=yes, ax_cv_cxx_have_complex_math2=no)
 LIBS="$ac_save_LIBS"
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_complex_math2" = yes; then
  AC_DEFINE(HAVE_COMPLEX_MATH2,,[define if the compiler has more complex math functions])
fi
])

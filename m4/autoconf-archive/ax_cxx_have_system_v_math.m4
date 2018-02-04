# ==============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_system_v_math.html
# ==============================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_SYSTEM_V_MATH
#
# DESCRIPTION
#
#   If the compiler has the double math functions _class, trunc, itrunc,
#   nearest, rsqrt, uitrunc, copysign, drem, finite, and unordered, define
#   HAVE_SYSTEM_V_MATH.
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

AU_ALIAS([AC_CXX_HAVE_SYSTEM_V_MATH], [AX_CXX_HAVE_SYSTEM_V_MATH])
AC_DEFUN([AX_CXX_HAVE_SYSTEM_V_MATH],
[AC_CACHE_CHECK(whether the compiler supports System V math library,
ax_cv_cxx_have_system_v_math,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 ac_save_LIBS="$LIBS"
 LIBS="$LIBS -lm"
 AC_TRY_LINK([
#ifndef _ALL_SOURCE
 #define _ALL_SOURCE
#endif
#ifndef _XOPEN_SOURCE
 #define _XOPEN_SOURCE
#endif
#ifndef _XOPEN_SOURCE_EXTENDED
 #define _XOPEN_SOURCE_EXTENDED 1
#endif
#include <math.h>],[double x = 1.0; double y = 1.0;
_class(x); trunc(x); finite(x); itrunc(x); nearest(x); rsqrt(x); uitrunc(x);
copysign(x,y); drem(x,y); unordered(x,y);
return 0;],
 ax_cv_cxx_have_system_v_math=yes, ax_cv_cxx_have_system_v_math=no)
 LIBS="$ac_save_LIBS"
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_system_v_math" = yes; then
  AC_DEFINE(HAVE_SYSTEM_V_MATH,,[define if the compiler supports System V math library])
fi
])

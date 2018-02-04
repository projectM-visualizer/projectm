# ===============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_numeric_limits.html
# ===============================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_NUMERIC_LIMITS
#
# DESCRIPTION
#
#   If the compiler has numeric_limits<T>, define HAVE_NUMERIC_LIMITS.
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

AU_ALIAS([AC_CXX_HAVE_NUMERIC_LIMITS], [AX_CXX_HAVE_NUMERIC_LIMITS])
AC_DEFUN([AX_CXX_HAVE_NUMERIC_LIMITS],
[AC_CACHE_CHECK(whether the compiler has numeric_limits<T>,
ax_cv_cxx_have_numeric_limits,
[AC_REQUIRE([AX_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <limits>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[double e = numeric_limits<double>::epsilon(); return 0;],
 ax_cv_cxx_have_numeric_limits=yes, ax_cv_cxx_have_numeric_limits=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_numeric_limits" = yes; then
  AC_DEFINE(HAVE_NUMERIC_LIMITS,,[define if the compiler has numeric_limits<T>])
fi
])

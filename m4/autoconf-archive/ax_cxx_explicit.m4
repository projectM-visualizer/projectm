# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_cxx_explicit.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_EXPLICIT
#
# DESCRIPTION
#
#   If the compiler can be asked to prevent using implicitly one argument
#   constructors as converting constructors with the explicit keyword,
#   define HAVE_EXPLICIT.
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

AU_ALIAS([AC_CXX_EXPLICIT], [AX_CXX_EXPLICIT])
AC_DEFUN([AX_CXX_EXPLICIT],
[AC_CACHE_CHECK(whether the compiler supports the explicit keyword,
ax_cv_cxx_explicit,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([class A{public:explicit A(double){}};],
[double c = 5.0;A x(c);return 0;],
 ax_cv_cxx_explicit=yes, ax_cv_cxx_explicit=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_explicit" = yes; then
  AC_DEFINE(HAVE_EXPLICIT,,[define if the compiler supports the explicit keyword])
fi
])

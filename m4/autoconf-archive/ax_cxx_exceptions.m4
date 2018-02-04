# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_cxx_exceptions.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_EXCEPTIONS
#
# DESCRIPTION
#
#   If the C++ compiler supports exceptions handling (try, throw and catch),
#   define HAVE_EXCEPTIONS.
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

AU_ALIAS([AC_CXX_EXCEPTIONS], [AX_CXX_EXCEPTIONS])
AC_DEFUN([AX_CXX_EXCEPTIONS],
[AC_CACHE_CHECK(whether the compiler supports exceptions,
ax_cv_cxx_exceptions,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(,[try { throw  1; } catch (int i) { return i; }],
 ax_cv_cxx_exceptions=yes, ax_cv_cxx_exceptions=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_exceptions" = yes; then
  AC_DEFINE(HAVE_EXCEPTIONS,,[define if the compiler supports exceptions])
fi
])

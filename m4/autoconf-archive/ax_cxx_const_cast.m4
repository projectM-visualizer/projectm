# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_cxx_const_cast.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_CONST_CAST
#
# DESCRIPTION
#
#   If the compiler supports const_cast<>, define HAVE_CONST_CAST.
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

#serial 8

AU_ALIAS([AC_CXX_CONST_CAST], [AX_CXX_CONST_CAST])
AC_DEFUN([AX_CXX_CONST_CAST],
[dnl
  AC_CACHE_CHECK([whether the compiler supports const_cast<>],
                 ax_cv_cxx_const_cast,
  [dnl
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE([dnl
      AC_LANG_PROGRAM([],
        [int x = 0;
         const int& y = x;
         int& z = const_cast<int&>(y);
         return z;])],
      ax_cv_cxx_const_cast=yes,
      ax_cv_cxx_const_cast=no)
    AC_LANG_POP([C++])
  ])
  AS_IF([test "X$ax_cv_cxx_const_cast" = Xyes],
    [AC_DEFINE(HAVE_CONST_CAST,,[define if the compiler supports const_cast<>])])
])

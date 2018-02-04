# =======================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_enum_computations_with_cast.html
# =======================================================================================
#
# SYNOPSIS
#
#   AX_CXX_ENUM_COMPUTATIONS_WITH_CAST
#
# DESCRIPTION
#
#   If the compiler handle (int) casts in enum computations, define
#   HAVE_ENUM_COMPUTATIONS_WITH_CAST.
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

AU_ALIAS([AC_CXX_ENUM_COMPUTATIONS_WITH_CAST], [AX_CXX_ENUM_COMPUTATIONS_WITH_CAST])
AC_DEFUN([AX_CXX_ENUM_COMPUTATIONS_WITH_CAST],
[AC_CACHE_CHECK(whether the compiler handles (int) casts in enum computations,
ax_cv_cxx_enum_computations_with_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
struct A { enum { a = 5, b = 7, c = 2 }; };
struct B { enum { a = 1, b = 6, c = 9 }; };
template<class T1, class T2> struct Z
{ enum { a = ((int)T1::a > (int)T2::a) ? (int)T1::a : (int)T2::b,
         b = (int)T1::b + (int)T2::b,
         c = ((int)T1::c * (int)T2::c + (int)T2::a + (int)T1::a)
       };
};],[
return (((int)Z<A,B>::a == 5)
     && ((int)Z<A,B>::b == 13)
     && ((int)Z<A,B>::c == 24)) ? 0 : 1;],
 ax_cv_cxx_enum_computations_with_cast=yes, ax_cv_cxx_enum_computations_with_cast=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_enum_computations_with_cast" = yes; then
  AC_DEFINE(HAVE_ENUM_COMPUTATIONS_WITH_CAST,,
            [define if the compiler handles (int) casts in enum computations])
fi
])

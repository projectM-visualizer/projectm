# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_old_for_scoping.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_OLD_FOR_SCOPING
#
# DESCRIPTION
#
#   If the compiler accepts the old for scoping rules (the scope of a
#   variable declared inside the parentheses extends outside the for-body),
#   define HAVE_OLD_FOR_SCOPING. Note that some compilers (notably g++ and
#   egcs) support both new and old rules since they accept the old rules and
#   only generate a warning.
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

AU_ALIAS([AC_CXX_OLD_FOR_SCOPING], [AX_CXX_OLD_FOR_SCOPING])
AC_DEFUN([AX_CXX_OLD_FOR_SCOPING],
[AC_CACHE_CHECK(whether the compiler accepts the old for scoping rules,
ax_cv_cxx_old_for_scoping,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(,[int z;for (int i=0; i < 10; ++i)z=z+i;z=i;return z;],
 ax_cv_cxx_old_for_scoping=yes, ax_cv_cxx_old_for_scoping=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_old_for_scoping" = yes; then
  AC_DEFINE(HAVE_OLD_FOR_SCOPING,,[define if the compiler accepts the old for scoping rules])
fi
])

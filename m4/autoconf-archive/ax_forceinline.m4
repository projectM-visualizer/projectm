# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_forceinline.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_FORCEINLINE()
#
# DESCRIPTION
#
#   Provides a test for C compiler support of forced inlining.  If usable,
#   #define FORCEINLINE to the appropriate force inline keyword.  Otherwise
#   #define FORCEINLINE to be 'inline'.
#
# LICENSE
#
#   Copyright (c) 2008 Alan Woodland <ajw05@aber.ac.uk>
#   Copyright (c) 2009 Rhys Ulerich <rhys.ulerich@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_FORCEINLINE], [
  AC_LANG_PUSH([C])
  AC_MSG_CHECKING(for forced inline keyword)
  AC_CACHE_VAL(ac_cv_forceinline, [
    ax_forceinline_keywords="__forceinline inline none"
    for ax_forceinline_keyword in $ax_forceinline_keywords; do
       case $ax_forceinline_keyword in
          none) ac_cv_forceinline=none ; break ;;
      *)
             AC_TRY_COMPILE(
                [#include <stdlib.h>
                 ] $ax_forceinline_keyword [
                 static void
                 foo(void) {
                 exit(1);
                 }],
                 [],
                 [ac_cv_forceinline=$ax_forceinline_keyword ; break],
                 ac_cv_forceinline=none
             )
      esac
    done
])

  if test "$ac_cv_forceinline" = "none"; then
    ax_forceinline_keyword=
  else
    ax_forceinline_keyword=$ac_cv_forceinline
  fi
  AC_DEFINE_UNQUOTED([FORCEINLINE],$ax_forceinline_keyword,
    [The most forceful inline keyword known by the compiler])
  AC_MSG_RESULT($ac_cv_forceinline)
  AC_LANG_POP([C])
])

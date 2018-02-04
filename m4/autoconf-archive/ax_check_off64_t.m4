# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_check_off64_t.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_OFF64_T
#
# DESCRIPTION
#
#   Check if off64_t is defined. On true define HAVE_OFF64_T, also define
#   __LARGEFILE64_SOURCE where one is needed. (Note that an appropriative
#   entry must be in config.h.in.)
#
# LICENSE
#
#   Copyright (c) 2008 Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([RSSH_CHECK_OFF64_T], [AX_CHECK_OFF64_T])
AC_DEFUN([AX_CHECK_OFF64_T], [
AC_REQUIRE([AC_SYS_LARGEFILE])dnl
AC_CHECK_HEADER(unistd.h)
AC_CACHE_CHECK([whether type off64_t support],
               [ax_cv_check_off64_t],
 [
  AC_COMPILE_IFELSE(
AC_LANG_SOURCE([
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
extern off64_t x1;
])
,ax_have_off64t=1)
  if test "x$ax_have_off64t" = "x"
   then
  AC_COMPILE_IFELSE(
AC_LANG_SOURCE([
#define _LARGEFILE64_SOURCE
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
extern off64_t x1;
]),
 ax_cv_check_off64_t="_LARGEFILE64_SOURCE",
 ax_cv_check_off64_t="no"
)dnl

   else
    ax_cv_check_off64_t=yes
   fi
 ])dnl

if test "x$ax_cv_check_off64_t" = "x_LARGEFILE64_SOURCE"
then
 AC_DEFINE(_LARGEFILE64_SOURCE)
 AC_DEFINE(HAVE_OFF64_T)
elif test "x$ax_cv_check_off64_t" = "xyes"
then
 AC_DEFINE(HAVE_OFF64_T)
fi
])dnl

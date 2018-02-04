# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_have_select.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_HAVE_SELECT([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#   AX_HAVE_PSELECT([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro determines whether the system supports the select I/O event
#   interface. A neat usage example would be:
#
#     AX_HAVE_SELECT(
#       [AX_CONFIG_FEATURE_ENABLE(select)],
#       [AX_CONFIG_FEATURE_DISABLE(select)])
#     AX_CONFIG_FEATURE(
#       [select], [This platform supports select(7)],
#       [HAVE_SELECT], [This platform supports select(7).])
#
#   Some systems also have the variant pselect(). The availability of that
#   function can be tested with the second macro. Generally speaking, it is
#   safe to assume that AX_HAVE_SELECT would succeed if AX_HAVE_SELECT_PWAIT
#   has, but not the other way round.
#
# LICENSE
#
#   Copyright (c) 2009 Peter Simons <simons@cryp.to>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_HAVE_SELECT], [dnl
  AC_MSG_CHECKING([for select(2)])
  AC_CACHE_VAL([ax_cv_have_select], [dnl
    AC_LINK_IFELSE([dnl
      AC_LANG_PROGRAM(
        [#include <sys/select.h>],
        [int rc; rc = select(0, (fd_set *)(0), (fd_set *)(0), (fd_set *)(0), (struct timeval *)(0));])],
      [ax_cv_have_select=yes],
      [ax_cv_have_select=no])])
  AS_IF([test "${ax_cv_have_select}" = "yes"],
    [AC_MSG_RESULT([yes])
$1],[AC_MSG_RESULT([no])
$2])
])dnl

AC_DEFUN([AX_HAVE_PSELECT], [dnl
  AC_MSG_CHECKING([for pselect(2)])
  AC_CACHE_VAL([ax_cv_have_pselect], [dnl
    AC_LINK_IFELSE([dnl
      AC_LANG_PROGRAM(
        [dnl
#include <sys/select.h>
#include <signal.h>],
        [dnl
int rc;
rc = select(0, (fd_set *)(0), (fd_set *)(0), (fd_set *)(0), (struct timeval *)(0));
rc = pselect(0, (fd_set *)(0), (fd_set *)(0), (fd_set *)(0), (struct timespec const *)(0), (sigset_t const *)(0));])],
      [ax_cv_have_pselect=yes],
      [ax_cv_have_pselect=no])])
  AS_IF([test "${ax_cv_have_pselect}" = "yes"],
    [AC_MSG_RESULT([yes])
$1],[AC_MSG_RESULT([no])
$2])
])dnl

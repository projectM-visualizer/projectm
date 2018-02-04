# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_check_posix_sysinfo.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_POSIX_SYSINFO
#
# DESCRIPTION
#
#   Check that the POSIX compliant sysinfo(2) call works properly. Linux has
#   its own weirdo alternative.
#
# LICENSE
#
#   Copyright (c) 2008 Bruce Korb <bkorb@gnu.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([AG_CHECK_POSIX_SYSINFO], [AX_CHECK_POSIX_SYSINFO])
AC_DEFUN([AX_CHECK_POSIX_SYSINFO],[
  AC_MSG_CHECKING([whether sysinfo(2) is POSIX])
  AC_CACHE_VAL([ax_cv_posix_sysinfo],[
  AC_TRY_RUN([#include <sys/systeminfo.h>
int main() { char z[ 256 ];
long sz = sysinfo( SI_SYSNAME, z, sizeof( z ));
return (sz > 0) ? 0 : 1; }],[ax_cv_posix_sysinfo=yes],[ax_cv_posix_sysinfo=no],[ax_cv_posix_sysinfo=no]
  ) # end of TRY_RUN]) # end of CACHE_VAL

  AC_MSG_RESULT([$ax_cv_posix_sysinfo])
  if test x$ax_cv_posix_sysinfo = xyes
  then
    AC_DEFINE(HAVE_POSIX_SYSINFO, 1,
       [Define this if sysinfo(2) is POSIX])
  fi
]) # end of AC_DEFUN of AX_CHECK_POSIX_SYSINFO

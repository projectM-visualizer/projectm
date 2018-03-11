# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_check_sys_siglist.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_SYS_SIGLIST
#
# DESCRIPTION
#
#   Check for existence of global sys_siglist[].
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

AU_ALIAS([AG_CHECK_SYS_SIGLIST], [AX_CHECK_SYS_SIGLIST])
AC_DEFUN([AX_CHECK_SYS_SIGLIST],[
  AC_MSG_CHECKING([whether there is a global text array sys_siglist])
  AC_CACHE_VAL([ax_cv_sys_siglist],[
  AC_TRY_RUN([#include <signal.h>
int main() {
  const char* pz = sys_siglist[1];
  return (pz != 0) ? 0 : 1; }],[ax_cv_sys_siglist=yes],[ax_cv_sys_siglist=no],[ax_cv_sys_siglist=no]
  ) # end of TRY_RUN]) # end of CACHE_VAL

  AC_MSG_RESULT([$ax_cv_sys_siglist])
  if test x$ax_cv_sys_siglist = xyes
  then
    AC_DEFINE(HAVE_SYS_SIGLIST, 1,
       [Define this if there is a global text array sys_siglist])
    NEED_SYS_SIGLIST=false
  else
    NEED_SYS_SIGLIST=true
  fi
  AC_SUBST(NEED_SYS_SIGLIST)
]) # end of AC_DEFUN of AX_CHECK_SYS_SIGLIST

# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_check_pathfind.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_PATHFIND
#
# DESCRIPTION
#
#   Not all systems have pathfind(3). See if we need to substitute. To make
#   this work, you have to do horrible things. See the doc for
#   AX_CHECK_STRCSPN.
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

AU_ALIAS([AG_CHECK_PATHFIND], [AX_CHECK_PATHFIND])
AC_DEFUN([AX_CHECK_PATHFIND],[
  AC_MSG_CHECKING([whether pathfind(3) works])
  AC_CACHE_VAL([ax_cv_pathfind],[
  AC_TRY_RUN([#include <string.h>
#include <stdlib.h>
int main (int argc, char** argv) {
   char* pz = pathfind( getenv( "PATH" ), "sh", "x" );
   return (pz == 0) ? 1 : 0;
}],[ax_cv_pathfind=yes],[ax_cv_pathfind=no],[ax_cv_pathfind=no]
  ) # end of TRY_RUN]) # end of CACHE_VAL

  AC_MSG_RESULT([$ax_cv_pathfind])
  if test x$ax_cv_pathfind = xyes
  then
    AC_DEFINE(HAVE_PATHFIND, 1,
       [Define this if pathfind(3) works])
  else
    if test x$ac_cv_lib_gen_pathfind = xyes
    then :
    else
      COMPATOBJ="$COMPATOBJ pathfind.lo"
    fi
  fi
]) # end of AC_DEFUN of AX_CHECK_PATHFIND

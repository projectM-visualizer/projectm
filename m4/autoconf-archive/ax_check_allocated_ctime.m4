# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_check_allocated_ctime.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_CHECK_ALLOCATED_CTIME
#
# DESCRIPTION
#
#   Check whether we need to free the memory returned by ctime.
#
# LICENSE
#
#   Copyright (c) 2008 Bruce Korb <bkorb@gnu.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([AG_CHECK_ALLOCATED_CTIME], [AX_CHECK_ALLOCATED_CTIME])
AC_DEFUN([AX_CHECK_ALLOCATED_CTIME],[
  AC_MSG_CHECKING([whether ctime() allocates memory for its result])
  AC_CACHE_VAL([ax_cv_allocated_ctime],[
  AC_TRY_RUN([#include <time.h>
int main (int argc, char** argv) {
   time_t  timeVal = time( (time_t*)NULL );
   char*   pzTime  = ctime( &timeVal );
   free( pzTime );
   return 0; }],[ax_cv_allocated_ctime=yes],[ax_cv_allocated_ctime=no],[ax_cv_allocated_ctime=no]
  ) # end of TRY_RUN]) # end of CACHE_VAL

  AC_MSG_RESULT([$ax_cv_allocated_ctime])
  if test x$ax_cv_allocated_ctime = xyes
  then
    AC_DEFINE(HAVE_ALLOCATED_CTIME, 1,
       [Define this if ctime() allocates memory for its result])
  fi
]) # end of AC_DEFUN of AX_CHECK_ALLOCATED_CTIME

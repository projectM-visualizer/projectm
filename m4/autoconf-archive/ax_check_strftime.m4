# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_check_strftime.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_STRFTIME
#
# DESCRIPTION
#
#   Check for existence and functioning of strftime routine.
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

AU_ALIAS([AG_CHECK_STRFTIME], [AX_CHECK_STRFTIME])
AC_DEFUN([AX_CHECK_STRFTIME],[
  AC_MSG_CHECKING([whether strftime() works])
  AC_CACHE_VAL([ax_cv_strftime],[
  AC_TRY_RUN([#include <time.h>
char t_buf[ 64 ];
int main() {
  static const char z[] = "Thursday Aug 28 240";
  struct tm tm;
  tm.tm_sec   = 36;  /* seconds after the minute [0, 61]  */
  tm.tm_min   = 44;  /* minutes after the hour [0, 59] */
  tm.tm_hour  = 12;  /* hour since midnight [0, 23] */
  tm.tm_mday  = 28;  /* day of the month [1, 31] */
  tm.tm_mon   =  7;  /* months since January [0, 11] */
  tm.tm_year  = 86;  /* years since 1900 */
  tm.tm_wday  =  4;  /* days since Sunday [0, 6] */
  tm.tm_yday  = 239; /* days since January 1 [0, 365] */
  tm.tm_isdst =  1;  /* flag for daylight savings time */
  strftime( t_buf, sizeof( t_buf ), "%A %b %d %j", &tm );
  return (strcmp( t_buf, z ) != 0); }],[ax_cv_strftime=yes],[ax_cv_strftime=no],[ax_cv_strftime=no]
  ) # end of TRY_RUN]) # end of CACHE_VAL

  AC_MSG_RESULT([$ax_cv_strftime])
  if test x$ax_cv_strftime = xyes
  then
    AC_DEFINE(HAVE_STRFTIME, 1,
       [Define this if strftime() works])
  fi
]) # end of AC_DEFUN of AX_CHECK_STRFTIME

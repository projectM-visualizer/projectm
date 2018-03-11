# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_c_long_long.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_C_LONG_LONG
#
# DESCRIPTION
#
#   Provides a test for the existence of the long long int type and defines
#   HAVE_LONG_LONG if it is found.
#
# LICENSE
#
#   Copyright (c) 2008 Caolan McNamara <caolan@skynet.ie>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([AC_C_LONG_LONG], [AX_C_LONG_LONG])
AC_DEFUN([AX_C_LONG_LONG],
[AC_CACHE_CHECK(for long long int, ac_cv_c_long_long,
[if test "$GCC" = yes; then
  ac_cv_c_long_long=yes
  else
        AC_TRY_COMPILE(,[long long int i;],
   ac_cv_c_long_long=yes,
   ac_cv_c_long_long=no)
   fi])
   if test $ac_cv_c_long_long = yes; then
     AC_DEFINE(HAVE_LONG_LONG, 1, [compiler understands long long])
   fi
])

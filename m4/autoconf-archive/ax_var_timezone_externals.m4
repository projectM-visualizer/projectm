# ==============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_var_timezone_externals.html
# ==============================================================================
#
# SYNOPSIS
#
#   AX_VAR_TIMEZONE_EXTERNALS
#
# DESCRIPTION
#
#   Use instead of `AC_STRUCT_TIMEZONE' to determine whether the external
#   timezone variables `timezone', `altzone' and `daylight' exist, defining
#   `HAVE_TIMEZONE', `HAVE_ALTZONE' and `HAVE_DAYLIGHT' respectively (as
#   well as gaining the macros defined by `AC_STRUCT_TIMEZONE').
#
#   Rewritten for Autoconf 2.68, and made robust against BSD's timezone()
#   function, by Daniel Richard G.
#
# LICENSE
#
#   Copyright (c) 2008 Mark R. Bannister <markb@freedomware.co.uk>
#   Copyright (c) 2011 Daniel Richard G. <skunk@iSKUNK.ORG>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 7

AU_ALIAS([AC_VAR_TIMEZONE_EXTERNALS], [AX_VAR_TIMEZONE_EXTERNALS])
AC_DEFUN([AX_VAR_TIMEZONE_EXTERNALS],
[  AC_REQUIRE([AC_STRUCT_TIMEZONE])dnl

   AC_LANG_PUSH([C])

   AC_CACHE_CHECK([for `timezone' variable in time.h], [mb_cv_var_timezone],
   [  AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <time.h>],
            [int *foo = (int *)0xdeadbeef; return foo@<:@timezone@:>@ /* ; */])],
         [mb_cv_var_timezone=yes],
         [mb_cv_var_timezone=no])
   ])

   AC_CACHE_CHECK([for `altzone' variable in time.h], [mb_cv_var_altzone],
   [  AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <time.h>],
            [int *foo = (int *)0xdeadbeef; return foo@<:@altzone@:>@ /* ; */])],
         [mb_cv_var_altzone=yes],
         [mb_cv_var_altzone=no])
   ])

   AC_CACHE_CHECK([for `daylight' variable in time.h], [mb_cv_var_daylight],
   [  AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <time.h>],
            [int *foo = (int *)0xdeadbeef; return foo@<:@daylight@:>@ /* ; */])],
         [mb_cv_var_daylight=yes],
         [mb_cv_var_daylight=no])
   ])

   if test "$mb_cv_var_timezone" = "yes"; then
      AC_DEFINE([HAVE_TIMEZONE], [1],
         [Define to 1 if you have the external `timezone' variable.])
   fi

   if test "$mb_cv_var_altzone" = "yes"; then
      AC_DEFINE([HAVE_ALTZONE], [1],
         [Define to 1 if you have the external `altzone' variable.])
   fi

   if test "$mb_cv_var_daylight" = "yes"; then
      AC_DEFINE([HAVE_DAYLIGHT], [1],
         [Define to 1 if you have the external `daylight' variable.])
   fi

   AC_LANG_POP
])

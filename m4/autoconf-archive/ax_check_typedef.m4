# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_check_typedef.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_TYPEDEF(TYPEDEF, HEADER [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]])
#
# DESCRIPTION
#
#   Check if the given typedef-name is recognized as a type. The trick is to
#   use a sizeof(TYPEDEF) and see if the compiler is happy with that.
#
#   This can be thought of as a mixture of AC_CHECK_TYPE(TYPEDEF,DEFAULT)
#   and AC_CHECK_LIB(LIBRARY,FUNCTION,ACTION-IF-FOUND,ACTION-IF-NOT-FOUND).
#
#   A convenience macro AX_CHECK_TYPEDEF_ is provided that will not emit any
#   message to the user - it just executes one of the actions.
#
# LICENSE
#
#   Copyright (c) 2008 Guido U. Draheim <guidod@gmx.de>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 3 of the License, or (at your
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

#serial 6

AU_ALIAS([AC_CHECK_TYPEDEF], [AX_CHECK_TYPEDEF])
AC_DEFUN([AX_CHECK_TYPEDEF_],
[dnl
ac_lib_var=`echo $1['_']$2 | sed 'y%./+-%__p_%'`
AC_CACHE_VAL(ac_cv_lib_$ac_lib_var,
[ eval "ac_cv_type_$ac_lib_var='not-found'"
  ac_cv_check_typedef_header=`echo ifelse([$2], , stddef.h, $2)`
  AC_TRY_COMPILE( [#include <$ac_cv_check_typedef_header>],
	[int x = sizeof($1); x = x;],
        eval "ac_cv_type_$ac_lib_var=yes" ,
        eval "ac_cv_type_$ac_lib_var=no" )
  if test `eval echo '$ac_cv_type_'$ac_lib_var` = "no" ; then
     ifelse([$4], , :, $4)
  else
     ifelse([$3], , :, $3)
  fi
])])

dnl AX_CHECK_TYPEDEF(TYPEDEF, HEADER [, ACTION-IF-FOUND,
dnl    [, ACTION-IF-NOT-FOUND ]])
AC_DEFUN([AX_CHECK_TYPEDEF],
[dnl
 AC_MSG_CHECKING([for $1 in $2])
 AX_CHECK_TYPEDEF_($1,$2,AC_MSG_RESULT(yes),AC_MSG_RESULT(no))dnl
])

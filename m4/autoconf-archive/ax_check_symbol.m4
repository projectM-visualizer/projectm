# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_check_symbol.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_SYMBOL(SYMBOL, HEADER... [,ACTION-IF-FOUND [,ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   A wrapper around AC_EGREP_HEADER. The shellvar $ac_found will hold the
#   HEADER-name that had been containing the symbol. This value is shown to
#   the user.
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

AU_ALIAS([AC_CHECK_SYMBOL], [AX_CHECK_SYMBOL])
AC_DEFUN([AX_CHECK_SYMBOL],
[AC_MSG_CHECKING([for $1 in $2])
AC_CACHE_VAL(ac_cv_func_$1,
[AC_REQUIRE_CPP()dnl
changequote(, )dnl
symbol="[^a-zA-Z_0-9]$1[^a-zA-Z_0-9]"
changequote([, ])dnl
ac_found=no
for ac_header in $2 ; do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%' `
  if test $ac_found != "yes" ; then
      if eval "test \"`echo '$ac_cv_header_'$ac_safe`\" = yes"; then
            AC_EGREP_HEADER( $symbol, $ac_header, [ac_found="$ac_header"] )
      fi
  fi
done
if test "$ac_found" != "no" ; then
  AC_MSG_RESULT($ac_found)
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
  ifelse([$4], , , [$4
])dnl
fi
])])

dnl AX_CHECK_SYMBOLS( symbol..., header... [, action-if-found [, action-if-not-found]])
AC_DEFUN([AX_CHECK_SYMBOLS],
[for ac_func in $1
do
P4_CHECK_SYMBOL($ac_func, $2,
[changequote(, )dnl
  ac_tr_func=HAVE_`echo $ac_func | sed -e 'y:abcdefghijklmnopqrstuvwxyz:ABCDEFGHIJKLMNOPQRSTUVWXYZ:' -e 's:[[^A-Z0-9]]:_:'`
changequote([, ])dnl
  AC_DEFINE_UNQUOTED($ac_tr_func) $2], $3)dnl
done
])

# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_very_nice.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_VERY_NICE
#
# DESCRIPTION
#
#   A macro to check the options of nice, in order to have a VERY_NICE
#   variable which runs a program at the lowest priority VERY_NICE is
#   undefined if we don't find the proper options, so you can safely use:
#
#     @VERY_NICE@ mycommand
#
#   in a shell script.
#
#   The VERY_NICE variable includes the placeholder NICE_VALUE that you have
#   to instantiate at run-time. If you give a argument to AX_VERY_NICE, it
#   will be used as an argument of nice for testing and included in
#   VERY_NICE instead of the above placeholder.
#
# LICENSE
#
#   Copyright (c) 2008 Stephane Bortzmeyer <bortzmeyer@pasteur.fr>
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

#serial 5

AU_ALIAS([AC_VERY_NICE], [AX_VERY_NICE])
AC_DEFUN([AX_VERY_NICE],[
if test "x$1" != "x"; then
  NICE_VALUE=$1
else
  NICE_VALUE=20
fi
AC_CHECK_PROGS(TEST_NICE, date)
AC_CHECK_PROGS(NICE, nice, )
AC_MSG_CHECKING(syntax of nice)
if test "x$NICE" != "x"; then
     if  ( $NICE -n $NICE_VALUE $TEST_NICE > /dev/null 2>&1 ) ;  then
        VERY_NICE="$NICE -n $NICE_VALUE"
     else
      if  ( $NICE -$NICE_VALUE $TEST_NICE > /dev/null 2>&1 ) ;  then
        VERY_NICE="$NICE -$NICE_VALUE"
      fi
     fi
fi
if test "x$1" = "x"; then
  VERY_NICE=`echo $VERY_NICE | sed "s/$NICE_VALUE/NICE_VALUE/"`
fi
AC_MSG_RESULT($VERY_NICE)
AC_SUBST(VERY_NICE)
])

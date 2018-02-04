# ================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_not_enable_frame_pointer.html
# ================================================================================
#
# SYNOPSIS
#
#   AX_NOT_ENABLE_FRAME_POINTER ([shellvar])
#
# DESCRIPTION
#
#   add --enable-frame-pointer option, the default will add the gcc
#   -fomit-frame-pointer option to the shellvar (per default CFLAGS) and
#   remove the " -g " debuginfo option from it. In other words, the default
#   is "--disable-frame-pointer"
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

#serial 9

AC_DEFUN([AX_NOT_ENABLE_FRAME_POINTER],[dnl
AS_VAR_PUSHDEF([VAR],[enable_frame_pointer])dnl
AC_MSG_CHECKING([m4_ifval($1,$1,CFLAGS) frame-pointer])
AC_ARG_ENABLE([frame-pointer], AS_HELP_STRING(
  [--enable-frame-pointer],[enable callframe generation for debugging]))
case ".$VAR" in
  .|.no|.no,*) test ".$VAR" = "." && VAR="no"
     m4_ifval($1,$1,CFLAGS)=`echo dnl
  " $m4_ifval($1,$1,CFLAGS) " | sed -e 's/ -g / /'`
     if test ".$GCC" = ".yes" ; then
        m4_ifval($1,$1,CFLAGS)="$m4_ifval($1,$1,CFLAGS) -fomit-frame-pointer"
        AC_MSG_RESULT([$VAR, -fomit-frame-pointer added])
     else
        AC_MSG_RESULT([$VAR, -g removed])
     fi  ;;
   *)  AC_MSG_RESULT([$VAR, kept]) ;;
esac
AS_VAR_POPDEF([VAR])dnl
])

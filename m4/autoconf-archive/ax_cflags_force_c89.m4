# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cflags_force_c89.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CFLAGS_FORCE_C89 [(shellvar [,default, [A/NA]])]
#
# DESCRIPTION
#
#   Try to find a compiler option that enables strict C89 mode.
#
#   For the GNU CC compiler it will be -ansi -pedantic.  The result is added
#   to the shellvar being CFLAGS by default.
#
#   Currently this macro knows about GCC, Solaris C compiler, Digital Unix C
#   compiler, C for AIX Compiler, HP-UX C compiler, IRIX C compiler, NEC
#   SX-5 (Super-UX 10) C compiler, and Cray J90 (Unicos 10.0.0.8) C
#   compiler.
#
#    - $1 shell-variable-to-add-to : CFLAGS
#    - $2 add-value-if-not-found : nothing
#    - $3 action-if-found : add value to shellvariable
#    - $4 action-if-not-found : nothing
#
#   NOTE: These macros depend on AX_APPEND_FLAG.
#
# LICENSE
#
#   Copyright (c) 2009 Guido U. Draheim <guidod@gmx.de>
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

#serial 10

AC_DEFUN([AX_CFLAGS_FORCE_C89],[dnl
AS_VAR_PUSHDEF([FLAGS],[CFLAGS])dnl
AS_VAR_PUSHDEF([VAR],[ac_cv_cflags_force_c89])dnl
AC_CACHE_CHECK([m4_ifval($1,$1,FLAGS) for C89 mode],
VAR,[VAR="no, unknown"
 AC_LANG_SAVE
 AC_LANG_C
 ac_save_[]FLAGS="$[]FLAGS"
for ac_arg dnl
in "-pedantic  % -ansi -pedantic"             dnl GCC
   "-xstrconst % -v -Xc"                      dnl Solaris C
   "-std1      % -std1"                       dnl Digital Unix
   " % -qlanglvl=ansi"                        dnl AIX
   " % -ansi -ansiE"                          dnl IRIX
   "+ESlit     % -Aa"                         dnl HP-UX C
   "-Xc        % -Xc"                         dnl NEC SX-5 (Super-UX 10)
   "-h conform % -h conform"                  dnl Cray C (Unicos)
   #
do FLAGS="$ac_save_[]FLAGS "`echo $ac_arg | sed -e 's,%%.*,,' -e 's,%,,'`
   AC_TRY_COMPILE([],[return 0;],
   [VAR=`echo $ac_arg | sed -e 's,.*% *,,'` ; break])
done
 FLAGS="$ac_save_[]FLAGS"
 AC_LANG_RESTORE
])
AS_VAR_POPDEF([FLAGS])dnl
AX_REQUIRE_DEFINED([AX_APPEND_FLAG])
case ".$VAR" in
     .ok|.ok,*) m4_ifvaln($3,$3) ;;
   .|.no|.no,*) m4_default($4,[m4_ifval($2,[AX_APPEND_FLAG([$2], [$1])])]) ;;
   *) m4_default($3,[AX_APPEND_FLAG([$VAR], [$1])]) ;;
esac
AS_VAR_POPDEF([VAR])dnl
])

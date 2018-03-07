# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_missing_prog.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_MISSING_PROG(VARIABLE,PROGRAM,[MESSAGE])
#
# DESCRIPTION
#
#   Simple wrapper for the AM_MISSING_PROG which displays a warning MESSAGE
#   if provided. A typical use is the following:
#
#     AC_PROG_AWK
#     AS_IF([test -z "$AWK"],[
#       AX_MISSING_PROG(AWK,[awk],[awk seems missing ...])
#     ])
#
#   Programs wrapped with AX_MISSING_PROG should be used only in the package
#   building phase (because they will be wrapped with the automake 'missing'
#   script, which will not be installed).
#
# LICENSE
#
#   Copyright (c) 2008 Francesco Salvestrini <salvestrini@users.sourceforge.net>
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

#serial 8

AC_DEFUN([AX_MISSING_PROG],[dnl
  AS_IF([ test ! -z "$3" ],[
    AC_MSG_WARN([$3])
  ])
  AM_MISSING_PROG([$1],[$2])
])

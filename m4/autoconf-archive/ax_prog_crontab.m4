# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_prog_crontab.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_CRONTAB
#
# DESCRIPTION
#
#   Check for the program crontab, if exists let script continue, else pops
#   an error message
#
#   Besides checking existence, this macro also set these environment
#   variables upon completion:
#
#     CRONTAB = which crontab
#
# LICENSE
#
#   Copyright (c) 2008 Gleen Salmon <gleensalmon@yahoo.com>
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

#serial 6

AU_ALIAS([AC_PROG_CRONTAB], [AX_PROG_CRONTAB])
AC_DEFUN([AX_PROG_CRONTAB],[
AC_REQUIRE([AC_EXEEXT])dnl
AC_PATH_PROG(CRONTAB, crontab$EXEEXT, nocommand)
if test "$CRONTAB" = nocommand; then
        AC_MSG_ERROR([crontab (needed for scheduled job) not found in $PATH])
fi;dnl
])

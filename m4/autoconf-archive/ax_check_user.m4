# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_check_user.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_USER
#
# DESCRIPTION
#
#   Check if the specified UNIX user exists, if yes set your environment
#   variable to that username else unset your environment variable
#
#   Example:
#
#     AX_CHECK_USER(USER, [gleensalmon])
#     if test x$USER = xgleensalmon; then
#         bla..bla..bla..
#     else
#         bla..bla..bla..
#     fi
#
#   Besides checking existence, this macro also set these environment
#   variables upon completion:
#
#     USER_HOME = home directory of user, written in /etc/passwd
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

AU_ALIAS([AC_CHECK_USER], [AX_CHECK_USER])
AC_DEFUN([AX_CHECK_USER],[
AC_MSG_CHECKING([for user $2])
if grep ^$2: /etc/passwd > /dev/null; then
        $1=$2
        USER_HOME=`grep ^$2: /etc/passwd | sed "s/^\([[^:]]*:\)\{5\}\([[^:]]*\):[[^:]]*$/\2/"`
        AC_MSG_RESULT([yes])
else
        unset $1
        unset USER_HOME
        AC_MSG_RESULT([no])
fi;dnl
])

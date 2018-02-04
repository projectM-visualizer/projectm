# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_prog_pgclient.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_PGCLIENT()
#
# DESCRIPTION
#
#   This macro searches for a program called 'pgclient'. If found the
#   variable $pgclient is set to its path. Else it is set to 0. An option is
#   added to the configure script for setting an search path instead of
#   $PATH ($pgclient_dir). If pgclient is necessary for installing your
#   program, you can do something like the following to stop configure with
#   an error if pgclient wasn't found:
#
#     if test "x$pgclient" = "x0"; then AC_MSG_ERROR([We need that to setup the database!]); fi
#
#   pgclient can be found at http://pgclient.freesources.org
#
# LICENSE
#
#   Copyright (c) 2008 Moritz Sinn <moritz@freesources.org>
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

AU_ALIAS([MS_PROG_PGCLIENT], [AX_PROG_PGCLIENT])
AC_DEFUN([AX_PROG_PGCLIENT], [
AC_ARG_WITH(pgclient, [  --with-pgclient=DIR                    where to find pgclient (default: $PATH)], [pgclient_dir=$withval], [pgclient_dir=$PATH])
AC_PATH_PROG([pgclient], [pgclient], [0], $pgclient_dir)
])dnl

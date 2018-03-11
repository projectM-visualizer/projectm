# ===========================================================================
#        https://www.gnu.org/software/autoconf-archive/ax_lib_wad.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_WAD
#
# DESCRIPTION
#
#   This macro searches for an installed WAD library.
#
# LICENSE
#
#   Copyright (c) 2008 Sebastian Huber <sebastian-huber@web.de>
#   Copyright (c) 2008 Alan W. Irwin
#   Copyright (c) 2008 Rafael Laboissiere <rafael@laboissiere.net>
#   Copyright (c) 2008 Andrew Collier
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

#serial 10

AU_ALIAS([AC_LIB_WAD], [AX_LIB_WAD])
AC_DEFUN([AX_LIB_WAD],
[
        AC_REQUIRE([AX_PYTHON_DEVEL])
        AC_ARG_ENABLE(wad,
        AS_HELP_STRING([--enable-wad], [enable wad module]),
        [
                case "${enableval}" in
                        no)     ;;
                        *)      if test "x${enableval}" = xyes;
                                then
                                        check_wad="yes"
                                fi ;;
                esac
        ], [])

        if test -n "$check_wad";
        then
                AC_CHECK_LIB(wadpy, _init, [WADPY=-lwadpy], [], $PYTHON_LDFLAGS $PYTHON_EXTRA_LIBS)
                AC_SUBST(WADPY)
        fi
])

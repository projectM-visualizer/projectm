# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_prog_tcl.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_TCL([min-version])
#
# DESCRIPTION
#
#   Searches for tcl (tclsh and wish) in PATH and checks which version is
#   installed. The macro bails out if either tcl is not found or the minimum
#   version is not satisfied, unless minimum version is "0".
#
#   Example:
#
#    AX_PROG_TCL
#
#   This checks for tcl and if not found, exits with an error. If found, it
#   prints tcl path and version number.
#
#    AX_PROG_TCL([8.0])
#
#   Checks for tcl and exits with an error if its not found or the version
#   is below 8.0.
#
# LICENSE
#
#   Copyright (c) 2008 David Cluytens
#   Copyright (c) 2008 Uwe Mayer <merkosh@hadiko.de>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
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

AC_DEFUN([AX_PROG_TCL], [
#-- check for tclsh in PATH
AC_PATH_PROG([TCLSH], [tclsh], [no])
if [(test x"$TCLSH" == x"no") && (test x"$1" != x"0")]; then
        AC_MSG_ERROR([tclsh not found])
fi

#-- check for wish in PATH
AC_PATH_PROG([WISH], [wish], [no])

#-- check vor tcl version
AC_MSG_CHECKING([tcl version])
version=`echo "puts [[set tcl_version]]" | tclsh -`
AC_MSG_RESULT([$version])

#-- compare tcl version with min-version
required=$1
if [(test x"$1" != x"") && (test "${required/./}" -gt "${version/./}")]; then
        AC_MSG_ERROR([tcl version $1 required])
fi
])dnl

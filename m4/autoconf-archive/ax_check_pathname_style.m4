# ============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_check_pathname_style.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_CHECK_PATHNAME_STYLE_DOS
#
# DESCRIPTION
#
#   Check if host OS uses DOS-style pathnames. This includes the use of
#   drive letters and backslashes. Under DOS, Windows, and OS/2, defines
#   HAVE_PATHNAME_STYLE_DOS and PATH_SEPARATOR to ';'. Otherwise, defines
#   PATH_SEPARATOR to ':'.
#
#   This macro depends on the AC_CANONICAL_HOST.
#
#   Use for enabling code to handle drive letters, backslashes in filenames
#   and semicolons in the PATH.
#
# LICENSE
#
#   Copyright (c) 2008 Mark Elbrecht
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

AU_ALIAS([ACX_CHECK_PATHNAME_STYLE], [AX_CHECK_PATHNAME_STYLE])
AC_DEFUN([AX_CHECK_PATHNAME_STYLE_DOS],
[AC_MSG_CHECKING(for Windows and DOS and OS/2 style pathnames)
AC_CACHE_VAL(ax_cv_pathname_style_dos,
[AC_REQUIRE([AC_CANONICAL_HOST])

ax_cv_pathname_style_dos="no"
case ${host_os} in
  *djgpp | *mingw32* | *emx*) ax_cv_pathname_style_dos="yes" ;;
esac
])
AC_MSG_RESULT($ax_cv_pathname_style_dos)
if test "$ax_cv_pathname_style_dos" = "yes"; then
  AC_DEFINE(HAVE_PATHNAME_STYLE_DOS,,[defined if running on a system with dos style paths])
  AC_DEFINE(PATH_SEPARATOR, ';')
else
  AC_DEFINE(PATH_SEPARATOR, ':')
fi
])

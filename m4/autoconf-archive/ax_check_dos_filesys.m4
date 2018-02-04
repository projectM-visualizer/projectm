# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_check_dos_filesys.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_DOS_FILESYS_LIMITATIONS
#
# DESCRIPTION
#
#   Check if the target is running on DOS. DOS doesn't allow a dot as the
#   first character, more than one dot, more than eight characters before a
#   dot, and just three letters after the dot. A DOS VM running under
#   Windows 9X does not have these restrictions. A DOS program can be
#   running in either environment, so its important to code accordingly.
#   Defines HAVE_DOS_FILESYS_LIMITATIONS if under DOS.
#
#   Use in conjunction with AX_CHECK_PATHNAME_STYLE_DOS.
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

#serial 5

AU_ALIAS([ACX_CHECK_DOS_FILESYS], [AX_CHECK_DOS_FILESYS])
AC_DEFUN([AX_CHECK_DOS_FILESYS_LIMITATIONS],
[AC_MSG_CHECKING(for potential DOS filename limitations)
AC_CACHE_VAL(ax_cv_dos_limitations,
[AC_REQUIRE([AC_CANONICAL_HOST])
ax_cv_dos_limitations="yes"
case ${host_os} in
  *dos | *djgpp) ax_cv_dos_limitations="yes"
esac
])
AC_MSG_RESULT($ax_cv_dos_limitations)
if test $ax_cv_dos_limitations = "yes"; then
  AC_DEFINE(HAVE_DOS_FILESYS_LIMITATIONS)
fi
])

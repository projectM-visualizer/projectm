# ===============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_sys_largefile_sensitive.html
# ===============================================================================
#
# SYNOPSIS
#
#   AX_SYS_LARGEFILE_SENSITIVE
#
# DESCRIPTION
#
#   Check whether the current system is sensitive to -Ddefines making off_t
#   having different types/sizes. Automatically define a config.h symbol
#   LARGEFILE_SENSITIVE if that is the case, otherwise leave everything as
#   is.
#
#   This macro builds on top of AC_SYS_LARGEFILE to detect whether special
#   options are needed to make the code use 64bit off_t - in many setups
#   this will also make the code use 64bit off_t immediately.
#
#   The common use of a LARGEFILE_SENSITIVE config.h-define is to rename
#   exported functions, usually adding a 64 to the original function name.
#   Such renamings are only needed on systems being both (a) 32bit off_t by
#   default and (b) implementing large.file extensions (as for unix98).
#
#   a renaming section could look like this:
#
#    #if defined LARGEFILE_SENSITIVE && _FILE_OFFSET_BITS+0 == 64
#    #define zzip_open zzip_open64
#    #define zzip_seek zzip_seek64
#    #endif
#
#   for libraries, it is best to take advantage of the prefix-config.h
#   macro, otherwise you want to export a renamed LARGEFILE_SENSITIVE in an
#   installed header file. -> see AX_PREFIX_CONFIG_H
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

#serial 10

AU_ALIAS([AC_SYS_LARGEFILE_SENSITIVE], [AX_SYS_LARGEFILE_SENSITIVE])
AC_DEFUN([AX_SYS_LARGEFILE_SENSITIVE],[dnl
AC_REQUIRE([AC_SYS_LARGEFILE])dnl
# we know about some internals of ac_sys_largefile here...
AC_MSG_CHECKING(whether system differentiates 64bit off_t by defines)
ac_cv_sys_largefile_sensitive="no"
if test ".${ac_cv_sys_file_offset_bits-no}${ac_cv_sys_large_files-no}" != ".nono"
then ac_cv_sys_largefile_sensitive="yes"
  AC_DEFINE(LARGEFILE_SENSITIVE, 1,
  [whether the system defaults to 32bit off_t but can do 64bit when requested])
fi
AC_MSG_RESULT([$ac_cv_sys_largefile_sensitive])
])

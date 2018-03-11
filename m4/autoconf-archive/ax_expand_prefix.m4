# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_expand_prefix.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_EXPAND_PREFIX
#
# DESCRIPTION
#
#   When $prefix and $exec_prefix are still set to NONE then set them to the
#   usual default values - being based on $ac_default_prefix. - this macro
#   can be AC_REQUIREd by other macros that need to compute values for
#   installation directories. It has been observed that it was done wrong
#   over and over again, so this is a bit more safe to do.
#
#   remember - setting exec_prefix='${prefix}' needs you interpolate
#   directories multiple times, it is not sufficient to just say
#   MYVAR="${datadir}/putter" but you do have to run `eval` a few times,
#   sth. like MYVAR=`eval "echo \"$MYVAR\""` done at least two times.
#
#   The implementation of this macro simply picks up the lines that would be
#   run at the start of AC_OUTPUT anyway to set the prefix/exec_prefix
#   defaults. Between AC_INIT and the first command to AC_REQUIRE this macro
#   you can set the two variables to something explicit instead. Probably,
#   any command to compute installation directories should be run _after_
#   AM_INIT_AUTOMAKE
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

AC_DEFUN([AX_EXPAND_PREFIX],[dnl
  # The prefix default can be set in configure.ac (otherwise it is /usr/local)
  test "x$prefix" = xNONE && prefix=$ac_default_prefix
  # Let make expand exec_prefix. Allows to override the makevar 'prefix' later
  test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
])

# ===================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_maintainer_mode_auto_silent.html
# ===================================================================================
#
# SYNOPSIS
#
#   AX_MAINTAINER_MODE_AUTO_SILENT
#
# DESCRIPTION
#
#   Set autotools to error/sleep settings so that they are not run when
#   being erroneously triggered. Likewise make libtool-silent when libtool
#   has been used.
#
#   I use the macro quite a lot since some automake versions have the
#   tendency to try to rerun some autotools on a mere make even when not
#   quite in --maintainer-mode. That is very annoying. Likewise, a user who
#   installs from source does not want to see doubled compiler messages.
#
#   I did not put an AC_REQUIRE(MAINTAINER_MODE) in here - should I?
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

#serial 9

AC_DEFUN([AX_MAINTAINER_MODE_AUTO_SILENT],[dnl
dnl ac_REQUIRE([am_MAINTAINER_MODE])dn
AC_MSG_CHECKING(auto silent in maintainer mode)
if test "$USE_MAINTAINER_MODE" = "no" ; then
   test ".$TIMEOUT" = "." && TIMEOUT="9"
   AUTOHEADER="sleep $TIMEOUT ; true || autoheader || skipped"
   AUTOMAKE="sleep $TIMEOUT ; true || automake || skipped"
   AUTOCONF="sleep $TIMEOUT ; true || autoconf || skipped"
   if test ".$LIBTOOL" != "." ; then
      LIBTOOL="$LIBTOOL --silent"
      AC_MSG_RESULT([libtool-silent, auto-sleep-9])
   else
      AC_MSG_RESULT([auto-sleep-9])
   fi
else
      AC_MSG_RESULT([no])
fi
])

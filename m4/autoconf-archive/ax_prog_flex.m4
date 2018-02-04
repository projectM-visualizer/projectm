# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_prog_flex.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_FLEX(ACTION-IF-TRUE,ACTION-IF-FALSE)
#
# DESCRIPTION
#
#   Check whether flex is the scanner generator. Run ACTION-IF-TRUE if
#   successful, ACTION-IF-FALSE otherwise
#
# LICENSE
#
#   Copyright (c) 2009 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#   Copyright (c) 2010 Diego Elio Petteno` <flameeyes@gmail.com>
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

#serial 13

AC_DEFUN([AX_PROG_FLEX], [
  AC_REQUIRE([AM_PROG_LEX])
  AC_REQUIRE([AC_PROG_EGREP])

  AC_CACHE_CHECK([if flex is the lexer generator],[ax_cv_prog_flex],[
    AS_IF([$LEX --version 2>/dev/null | $EGREP -qw '^g?flex'],
      [ax_cv_prog_flex=yes], [ax_cv_prog_flex=no])
  ])
  AS_IF([test "$ax_cv_prog_flex" = "yes"],
    m4_ifnblank([$1], [[$1]]),
    m4_ifnblank([$2], [[$2]])
  )
])

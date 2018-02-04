# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_prog_emacs.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_EMACS
#
# DESCRIPTION
#
#   This macro allows the end user to specify a particular Emacs executable
#   via a configure script command-line arg. For example:
#
#     ./configure EMACS=$HOME/build/GNU/emacs/src/emacs
#
#   It also arranges to mention env var EMACS in the './configure --help'
#   output. See info node "(autoconf) Generic Programs" for details.
#
#   More precisely...
#
#   If env var EMACS is set, try to use its value directly, but avoid
#   getting fooled by value 't' (set by older Emacsen for subprocesses). If
#   no joy from the environment, search for "emacs" via AC_CHECK_PROG. If
#   still no joy, display "Emacs not found; required!" and make configure
#   exit failurefully. Otherwise, set shell var EMACS and AC_SUBST it, too.
#
# LICENSE
#
#   Copyright (c) 2016-2017 Thien-Thi Nguyen
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

#serial 2

AC_DEFUN([AX_PROG_EMACS],[dnl
AC_ARG_VAR([EMACS],[Use this Emacs to byte-compile the Emacs Lisp files.])
dnl Allow env override but do not get fooled by 'EMACS=t'.
test t = "$EMACS" && unset EMACS
dnl The next line does nothing if var 'EMACS' is already set.
AC_CHECK_PROG([EMACS], [emacs], [emacs])
AS_IF([test "x$EMACS" = x],[AC_MSG_ERROR([Emacs not found; required!])])
])dnl

dnl ax_prog_emacs.m4 ends here

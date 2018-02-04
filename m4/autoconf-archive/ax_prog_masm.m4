# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_prog_masm.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_MASM([ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro searches for the MASM assembler and sets the variable "masm"
#   to the name of the application or to "no" if not found. If
#   ACTION-IF-NOT-FOUND is not specified, configure will fail when the
#   program is not found.
#
#   Example:
#
#     AX_PROG_MASM()
#     AX_PROG_MASM([masm_avail="no"])
#
# LICENSE
#
#   Copyright (c) 2007,2009 Bogdan Drozdowski <bogdandr@op.pl>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU Lesser General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or (at
#   your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
#   General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program. If not, see <https://www.gnu.org/licenses/>.
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

AC_DEFUN([AX_PROG_MASM],[
AC_CHECK_PROGS(masm,[ml masm ml32 ml64 masm32],no)
if test "x$masm" = "xno" ;
then
	ifelse($#,0,[AC_MSG_ERROR([MASM assembler not found])],
        $1)
fi
])

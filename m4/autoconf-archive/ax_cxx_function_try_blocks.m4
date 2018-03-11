# ===============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_function_try_blocks.html
# ===============================================================================
#
# SYNOPSIS
#
#   AX_CXX_FUNCTION_TRY_BLOCKS
#
# DESCRIPTION
#
#   If the C++ compiler supports function try blocks, define
#   `HAVE_FUNCTION_TRY_BLOCKS'.
#
# LICENSE
#
#   Copyright (c) 2008 Matthew D. Langston
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

AU_ALIAS([MDL_CXX_FUNCTION_TRY_BLOCKS], [AX_CXX_FUNCTION_TRY_BLOCKS])
AC_DEFUN([AX_CXX_FUNCTION_TRY_BLOCKS],
[
AC_REQUIRE([AC_PROG_CXX])
changequote(,)dnl
AC_MSG_CHECKING(whether ${CXX} supports function try blocks)
changequote([,])dnl
AC_CACHE_VAL(ax_cv_have_function_try_blocks,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([void foo() try{} catch( ... ){}],
[foo();],
ax_cv_have_function_try_blocks=yes,
ax_cv_have_function_try_blocks=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ax_cv_have_function_try_blocks)
if test "$ax_cv_have_function_try_blocks" = yes; then
AC_DEFINE(HAVE_FUNCTION_TRY_BLOCKS)
fi])

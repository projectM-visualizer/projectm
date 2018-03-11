# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_var_prettyfunc.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_VAR_PRETTYFUNC
#
# DESCRIPTION
#
#   This function tries to determine the best C++ macro/identifier that
#   contains the current function name. Depending on the compiler, this may
#   be __PRETTY_FUNCTION__ (GCC), __FUNCSIG__ (MSVC), __func__ (C++
#   standard), __FUNCTION__ (fallback).
#
#   The function will define HAVE_PRETTYFUNC if a macro exists, and define
#   __PRETTYFUNC__ to the best possible macro.  When HAVE_PRETTYFUNC is not
#   defined, __PRETTYFUNC__ will contain the constant string "<<unknown>>".
#
# LICENSE
#
#   Copyright (c) 2014 Olaf Lenz <olenz@icp.uni-stuttgart.de>
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

#serial 2

AC_DEFUN([AX_CXX_VAR_PRETTYFUNC],
[
# try to find best __FUNCTION__ variant

AC_CACHE_CHECK([whether $CXX can get a pretty function name], ac_cv_cxx_var_prettyfunc,
    ac_cv_cxx_var_prettyfunc=no

    AC_COMPILE_IFELSE(
          [AC_LANG_PROGRAM(,[const char *s = __PRETTY_FUNCTION__])],
        [ ac_cv_cxx_var_prettyfunc=__PRETTY_FUNCTION__ ])

    AS_IF([test "x$ac_cv_cxx_var_prettyfunc" = "xno"], [
        AC_COMPILE_IFELSE(
              [AC_LANG_PROGRAM(,[const char *s = __FUNCSIG__])],
            [ ac_cv_cxx_var_prettyfunc=__FUNCSIG__ ])
    ])

    AS_IF([test "x$ac_cv_cxx_var_prettyfunc" = "xno"], [
        AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM(,[const char *s = __func__])],
                       [ ac_cv_cxx_var_prettyfunc=__func__ ])
    ])

    AS_IF([test "x$ac_cv_cxx_var_prettyfunc" = "xno"], [
        AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM(,[const char *s = __FUNCTION__])],
                       [ ac_cv_cxx_var_prettyfunc=__FUNCTION__ ])
    ])
)


AS_IF([test "x$ac_cv_cxx_var_prettyfunc" != "xno"], [
    AC_DEFINE(HAVE_PRETTYFUNC,, [Whether __PRETTY_FUNCTION__ has a useful value.])
    AC_DEFINE_UNQUOTED(__PRETTYFUNC__,$ac_cv_cxx_var_prettyfunc,
        [contains the function wherein the macro is called])
],[
    AC_DEFINE(__PRETTYFUNC__,"<<unknown>>")
])

])dnl

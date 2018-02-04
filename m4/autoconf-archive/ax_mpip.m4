# ===========================================================================
#         https://www.gnu.org/software/autoconf-archive/ax_mpip.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_MPIP([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Test for mpiP: Lightweight, Scalable MPI Profiling
#   (http://mpip.sourceforge.net/)
#
#   Provides a --with-mpiP=DIR option. Searches --with-mpiP location and the
#   usual places for mpiP and required libraries.  Marks MPIP as a precious
#   environment variable.
#
#   On success, sets MPIP_LDFLAGS, MPIP_LIBS, #defines HAVE_MPIP, and calls
#   ACTION-IF-FOUND. When ACTION-IF-NOT-FOUND is not specified, the default
#   behavior is for configure to fail.
#
# LICENSE
#
#   Copyright (c) 2010 Rhys Ulerich <rhys.ulerich@gmail.com>
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
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

#serial 3

AC_DEFUN([AX_MPIP], [
AC_PREREQ(2.60)
AC_REQUIRE([ACX_MPI])
AC_ARG_VAR([MPIP],[mpiP options])

AC_ARG_WITH(
    [mpiP],
    [AS_HELP_STRING([--with-mpiP@<:@=DIR@:>@],
                    [use mpiP profiler; optional install prefix])],
    [with_mpiP=$withval],
    [with_mpiP=no])

if test "${with_mpiP}" != no ; then
    ax_mpiP_save_CC="$CC"
    ax_mpiP_save_LDFLAGS="$LDFLAGS"
    ax_mpiP_save_LIBS="$LIBS"

    MPIP_LDFLAGS=""
    MPIP_LIBS="-lmpiP -lbfd -liberty"
    if test "${with_mpiP}" != yes; then
        if test -d "${with_mpiP}/lib"; then
            MPIP_LDFLAGS="-L${with_mpiP}/lib"
        else
            AC_MSG_WARN([--with-mpiP expected ${with_mpiP}/lib to be a directory])
        fi
    fi
    LDFLAGS="${MPIP_LDFLAGS} ${LDFLAGS}"
    CC=$MPICC

    AC_LANG_PUSH([C])
    AC_CHECK_LIB([mpiP],
                 [mpiPi],
                 [ax_mpiP_cv_found=yes],
                 [ax_mpiP_cv_found=no],
                 [-lbfd -liberty])
    AC_LANG_POP([C])

    CC="$ax_mpiP_save_CC"
    LDFLAGS="$ax_mpiP_save_LDFLAGS"
    LIBS="$ax_mpiP_save_LIBS"

    if test "$ax_mpiP_cv_found" = no; then
        ifelse([$2],,AC_MSG_ERROR([mpiP not found.  Try --with-mpiP=DIR.]),
            [$2])
    else
        AC_DEFINE(HAVE_MPIP,1,[Define if mpiP is available])
        AC_SUBST(MPIP_LDFLAGS)
        AC_SUBST(MPIP_LIBS)
        ifelse([$1],,,[$1])
    fi
fi
])dnl AX_MPIP

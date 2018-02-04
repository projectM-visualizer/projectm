# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_trilinos_base.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_TRILINOS_BASE([, MINIMUM-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
#
# DESCRIPTION
#
#   Test for the Trilinos (http://trilinos.sandia.gov/) libraries.
#
#   Provides a --with-trilinos=DIR option. Searches --with-trilinos,
#   $TRILINOS_HOME, and the usual places for Trilinos installation headers.
#
#   Requires that a development branch or released version greater than
#   MINIMUM-VERSION be found. If not specified, the default minimum version
#   is 8.0.0.
#
#   Supports separately specifying --with-trilinos-include or
#   --with-trilinos-libdir to override default locations underneath either
#   --with-trilinos or $TRILINOS_HOME.
#
#   On success, adds -Ipath to CPPFLAGS, -Lpath to LDFLAGS, sets the
#   variable TRILINOS_INCLUDE based on the discovered location of
#   Trilinos_version.h, and #defines HAVE_TRILINOS. When ACTION-IF-NOT-FOUND
#   is not specified, the default behavior is for configure to fail.
#
# LICENSE
#
#   Copyright (c) 2009 Rhys Ulerich <rhys.ulerich@gmail.com>
#   Copyright (c) 2009 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2009 Caolan McNamara <caolan@skynet.ie>
#   Copyright (c) 2009 Alexandre Duret-Lutz <adl@gnu.org>
#   Copyright (c) 2009 Matthew Mueller <donut@azstarnet.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AC_DEFUN([AX_TRILINOS_BASE],
[
AC_REQUIRE([AX_TRILINOS_ABSOLUTE_HEADER])
AC_ARG_VAR(TRILINOS_HOME,[root directory of Trilinos installation])

AC_ARG_WITH(trilinos, [AS_HELP_STRING([--with-trilinos[=DIR]],[root directory of Trilinos installation])],[
with_trilinos=$withval
if test "${with_trilinos}" != yes; then
    TRILINOS_HOME=$withval
    trilinos_include="$withval/include"
    trilinos_libdir="$withval/lib"
fi
],[
with_trilinos=$withval
if test "x${TRILINOS_HOME}" != "x"; then
    trilinos_include="${TRILINOS_HOME}/include"
    trilinos_libdir="${TRILINOS_HOME}/lib"
fi
])

AC_ARG_WITH(trilinos-include,
[AS_HELP_STRING([--with-trilinos-include=DIR],[specify exact directory for Trilinos headers])],[
if test -d "$withval"; then
    trilinos_include="$withval"
else
    AC_MSG_ERROR([--with-trilinos-include expected directory name])
fi
])

AC_ARG_WITH(trilinos-libdir, [AS_HELP_STRING([--with-trilinos-libdir=DIR],[specify exact directory for Trilinos libraries])],[
if test -d "$withval"; then
    trilinos_libdir="$withval"
else
    AC_MSG_ERROR([--with-trilinos-libdir expected directory name])
fi
])

if test "${with_trilinos}" != no ; then

    OLD_LIBS=$LIBS
    OLD_LDFLAGS=$LDFLAGS
    OLD_CPPFLAGS=$CPPFLAGS

    if test -d "${trilinos_libdir}" ; then
        LDFLAGS="-L${trilinos_libdir} $LDFLAGS"
    fi
    if test -d "${trilinos_include}" ; then
        CPPFLAGS="-I${trilinos_include} $CPPFLAGS"
    fi

    succeeded=no
    AC_CHECK_HEADER([Trilinos_version.h],[found_header=yes],[found_header=no])
    if test "$found_header" = yes; then
        dnl Patterned after AX_BOOST_BASE
        trilinos_lib_version_req=ifelse([$1],,8.0.0,$1)
        trilinos_lib_version_req_shorten=`expr $trilinos_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
        trilinos_lib_version_req_major=`expr $trilinos_lib_version_req : '\([[0-9]]*\)'`
        trilinos_lib_version_req_minor=`expr $trilinos_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        trilinos_lib_version_req_sub_minor=`expr $trilinos_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$trilinos_lib_version_req_sub_minor" = "x" ; then
            trilinos_lib_version_req_sub_minor="0"
        fi
        WANT_TRILINOS_VERSION=`expr $trilinos_lib_version_req_major \* 10000 \+  $trilinos_lib_version_req_minor \* 100 \+ $trilinos_lib_version_req_sub_minor`
        AC_LANG_PUSH([C++])
        AC_MSG_CHECKING(for Trilinos release >= $trilinos_lib_version_req)
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
            @%:@include <Trilinos_version.h>
            ]], [[
            #if (TRILINOS_MAJOR_VERSION == 0)
            /* Development branch has zero major version.  A-OK. */
            #elif (TRILINOS_MAJOR_MINOR_VERSION >= $WANT_TRILINOS_VERSION)
            /* Stable release of appropriate version.  A-OK. */
            #else
            #  error Trilinos version is too old
            #endif
        ]])],[
            AC_MSG_RESULT(yes)
            succeeded=yes
        ],[
            AC_MSG_RESULT(no)
        ])
        AC_LANG_POP([C++])
    fi

    if test "$succeeded" = no; then
        LIBS=$OLD_LIBS
        LDFLAGS=$OLD_LDFLAGS
        CPPFLAGS=$OLD_CPPFLAGS

        ifelse([$3],,AC_MSG_ERROR([Trilinos not found.  Try either --with-trilinos or setting TRILINOS_HOME.]),
            [$3])
    else
        dnl Find the absolute path to Trilinos_version.h
        dnl We need it to back out the discovered TRILINOS_INCLUDE directory.
        AX_TRILINOS_ABSOLUTE_HEADER([Trilinos_version.h])
        TRILINOS_INCLUDE=`AS_DIRNAME([$ax_cv_absolute_Trilinos_version_h])`

        AC_DEFINE(HAVE_TRILINOS,1,[Define if Trilinos is available])
        AC_SUBST(TRILINOS_INCLUDE)
        ifelse([$2],,,[$2])
    fi

fi

])

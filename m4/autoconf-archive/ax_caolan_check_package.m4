# ============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_caolan_check_package.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_caolan_CHECK_PACKAGE(PACKAGE, FUNCTION, LIBRARY , HEADERFILE [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Provides --with-PACKAGE, --with-PACKAGE-include and
#   --with-PACKAGE-libdir options to configure. Supports the now standard
#   --with-PACKAGE=DIR approach where the package's include dir and lib dir
#   are underneath DIR, but also allows the include and lib directories to
#   be specified separately
#
#   adds the extra -Ipath to CFLAGS if needed adds extra -Lpath to LD_FLAGS
#   if needed searches for the FUNCTION in the LIBRARY with AC_CHECK_LIBRARY
#   and thus adds the lib to LIBS
#
#   defines HAVE_PKG_PACKAGE if it is found, (where PACKAGE in the
#   HAVE_PKG_PACKAGE is replaced with the actual first parameter passed)
#   note that autoheader will complain of not having the HAVE_PKG_PACKAGE
#   and you will have to add it to acconfig.h manually
#
# LICENSE
#
#   Copyright (c) 2008 Caolan McNamara <caolan@skynet.ie>
#   Copyright (c) 2008 Alexandre Duret-Lutz <adl@gnu.org>
#   Copyright (c) 2008 Matthew Mueller <donut@azstarnet.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_caolan_CHECK_PACKAGE],
[

AC_ARG_WITH($1,
[  --with-$1[=DIR]	root directory of $1 installation],
with_$1=$withval
if test "${with_$1}" != yes; then
        $1_include="$withval/include"
        $1_libdir="$withval/lib"
fi
)

AC_ARG_WITH($1-include,
[  --with-$1-include=DIR        specify exact include dir for $1 headers],
$1_include="$withval")

AC_ARG_WITH($1-libdir,
[  --with-$1-libdir=DIR        specify exact library dir for $1 library
  --without-$1        disables $1 usage completely],
$1_libdir="$withval")

if test "${with_$1}" != no ; then
        OLD_LIBS=$LIBS
        OLD_LDFLAGS=$LDFLAGS
        OLD_CFLAGS=$CFLAGS
        OLD_CPPFLAGS=$CPPFLAGS

        if test "${$1_libdir}" ; then
                LDFLAGS="$LDFLAGS -L${$1_libdir}"
        fi
        if test "${$1_include}" ; then
                CPPFLAGS="$CPPFLAGS -I${$1_include}"
                CFLAGS="$CFLAGS -I${$1_include}"
        fi

        no_good=no
        AC_CHECK_LIB($3,$2,,no_good=yes)
        AC_CHECK_HEADER($4,,no_good=yes)
        if test "$no_good" = yes; then
dnl	broken
                ifelse([$6], , , [$6])

                LIBS=$OLD_LIBS
                LDFLAGS=$OLD_LDFLAGS
                CPPFLAGS=$OLD_CPPFLAGS
                CFLAGS=$OLD_CFLAGS
        else
dnl	fixed
                ifelse([$5], , , [$5])

                AC_DEFINE(HAVE_PKG_$1)
        fi

fi

])

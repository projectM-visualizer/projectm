# ===========================================================================
#         https://www.gnu.org/software/autoconf-archive/ax_czmq.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CZMQ([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the CZMQ libraries of a particular version (or newer). The
#   default version tested for is 3.0.0.
#
#   The macro tests for CZMQ libraries in the library/include path, and,
#   when provided, also in the path given by --with-czmq.
#
#   This macro calls:
#
#     AC_SUBST(CZMQ_CPPFLAGS) / AC_SUBST(CZMQ_LDFLAGS) / AC_SUBST(CZMQ_LIBS)
#
#   And sets:
#
#     HAVE_CZMQ
#
# LICENSE
#
#   Copyright (c) 2016 Jeroen Meijer <jjgmeijer@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_CZMQ], [
    AC_ARG_WITH([czmq], [AS_HELP_STRING([--with-czmq=<prefix>],[CZMQ prefix directory])], [
        CZMQ_LDFLAGS="-L${with_czmq}/lib"
        CZMQ_CPPFLAGS="-I${with_czmq}/include"
    ])

    HAVE_CZMQ=0
    if test "$with_czmq" != "no"; then

        LD_FLAGS="$LDFLAGS $CZMQ_LDFLAGS"
        CPPFLAGS="$CPPFLAGS $CZMQ_CPPFLAGS"

        AC_LANG_SAVE
        AC_LANG_C
        AC_CHECK_HEADER(czmq.h, [czmq_h=yes], [czmq_h=no])
        AC_LANG_RESTORE

        if test "$czmq_h" = "yes"; then
            version=ifelse([$1], ,3.0.0,$1)
            AC_MSG_CHECKING([for CZMQ version >= $version])
            version=$(echo $version | tr '.' ',')
            AC_EGREP_CPP([version_ok], [
#include <czmq.h>
#if defined(CZMQ_VERSION) && CZMQ_VERSION >= CZMQ_MAKE_VERSION($version)
    version_ok
#endif
            ],[
                AC_MSG_RESULT(yes)
                HAVE_CZMQ=1
                CZMQ_LIBS="-lczmq"
                AC_SUBST(CZMQ_LDFLAGS)
                AC_SUBST(CZMQ_CPPFLAGS)
                AC_SUBST(CZMQ_LIBS)
            ], AC_MSG_RESULT([no valid CZMQ version was found]))
        else
            AC_MSG_WARN([no valid CZMQ installation was found])
        fi

        if test $HAVE_CZMQ = 1; then
            # execute ACTION-IF-FOUND (if present):
            ifelse([$2], , :, [$2])
        else
            # execute ACTION-IF-NOT-FOUND (if present):
            ifelse([$3], , :, [$3])
        fi
    else
        AC_MSG_NOTICE([not checking for CZMQ])
    fi

    AC_DEFINE(HAVE_CZMQ,,[define if the CZMQ library is available])
])

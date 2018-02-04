# ===========================================================================
#          https://www.gnu.org/software/autoconf-archive/ax_zmq.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_ZMQ([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the ZMQ libraries of a particular version (or newer). The
#   default version tested for is 4.0.0.
#
#   The macro tests for ZMQ libraries in the library/include path, and, when
#   provided, also in the path given by --with-zmq.
#
#   This macro calls:
#
#     AC_SUBST(ZMQ_CPPFLAGS) / AC_SUBST(ZMQ_LDFLAGS) / AC_SUBST(ZMQ_LIBS)
#
#   And sets:
#
#     HAVE_ZMQ
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

AC_DEFUN([AX_ZMQ], [
    AC_ARG_WITH([zmq], [AS_HELP_STRING([--with-zmq=<prefix>],[ZMQ prefix directory])], [
        ZMQ_LDFLAGS="-L${with_zmq}/lib"
        ZMQ_CPPFLAGS="-I${with_zmq}/include"
    ])

    HAVE_ZMQ=0
    if test "$with_zmq" != "no"; then

        LD_FLAGS="$LDFLAGS $ZMQ_LDFLAGS"
        CPPFLAGS="$CPPFLAGS $ZMQ_CPPFLAGS"

        AC_LANG_SAVE
        AC_LANG_C
        AC_CHECK_HEADER(zmq.h, [zmq_h=yes], [zmq_h=no])
        AC_LANG_RESTORE

        if test "$zmq_h" = "yes"; then
            version=ifelse([$1], ,4.0.0,$1)
            AC_MSG_CHECKING([for ZMQ version >= $version])
            version=$(echo $version | tr '.' ',')
            AC_EGREP_CPP([version_ok], [
#include <zmq.h>
#if defined(ZMQ_VERSION) && ZMQ_VERSION >= ZMQ_MAKE_VERSION($version)
    version_ok
#endif
            ],[
                AC_MSG_RESULT(yes)
                HAVE_ZMQ=1
                ZMQ_LIBS="-lzmq"
                AC_SUBST(ZMQ_LDFLAGS)
                AC_SUBST(ZMQ_CPPFLAGS)
                AC_SUBST(ZMQ_LIBS)
            ], AC_MSG_RESULT([no valid ZMQ version was found]))
        else
            AC_MSG_WARN([no valid ZMQ installation was found])
        fi

        if test $HAVE_ZMQ = 1; then
            # execute ACTION-IF-FOUND (if present):
            ifelse([$2], , :, [$2])
        else
            # execute ACTION-IF-NOT-FOUND (if present):
            ifelse([$3], , :, [$3])
        fi
    else
        AC_MSG_NOTICE([not checking for ZMQ])
    fi

    AC_DEFINE(HAVE_ZMQ,,[define if the ZMQ library is available])
])

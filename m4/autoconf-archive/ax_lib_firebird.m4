# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_lib_firebird.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_FIREBIRD([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the Firebird client library of a particular version (or newer).
#   This macro takes only one optional argument, the required version of
#   Firebird library. If required version is not passed, then 1.5.0 is used
#   in test of existence of Firebird client library.
#
#   For more information about Firebird API versioning check: API Identifies
#   Client Version http://www.firebirdsql.org/rlsnotes20/rnfbtwo-apiods.html
#
#   If no installation prefix to the installed Firebird library is given the
#   macro searches under /usr, /usr/local, and /opt.
#
#   This macro calls:
#
#     AC_SUBST(FIREBIRD_CFLAGS)
#     AC_SUBST(FIREBIRD_LDFLAGS)
#     AC_SUBST(FIREBIRD_VERSION)
#
#   And sets:
#
#     HAVE_FIREBIRD
#
# LICENSE
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 13

AC_DEFUN([AX_LIB_FIREBIRD],
[
    AC_ARG_WITH([firebird],
        AS_HELP_STRING(
            [--with-firebird=@<:@ARG@:>@],
            [use Firebird client library @<:@default=yes@:>@, optionally specify the prefix for firebird library]
        ),
        [
        if test "$withval" = "no"; then
            WANT_FIREBIRD="no"
        elif test "$withval" = "yes"; then
            WANT_FIREBIRD="yes"
            ac_firebird_path=""
        else
            WANT_FIREBIRD="yes"
            ac_firebird_path="$withval"
        fi
        ],
        [WANT_FIREBIRD="yes"]
    )

    FIREBIRD_CFLAGS=""
    FIREBIRD_LDFLAGS=""
    FIREBIRD_VERSION=""

    if test "x$WANT_FIREBIRD" = "xyes"; then

        ac_firebird_header="ibase.h"

        firebird_version_req=ifelse([$1], [], [3.0.0], [$1])
        firebird_version_req_shorten=`expr $firebird_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
        firebird_version_req_major=`expr $firebird_version_req : '\([[0-9]]*\)'`
        firebird_version_req_minor=`expr $firebird_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        firebird_version_req_micro=`expr $firebird_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$firebird_version_req_micro" = "x" ; then
            firebird_version_req_micro="0"
        fi

        dnl FB_API_VER represents the version of Firebird as follows:
        dnl - Any version of Interbase, or Firebird 1.0.x:      10
        dnl - Firebird 1.5.x:                                   15
        dnl - Firebird 2.0.x:                                   20

        firebird_version_req_number=`expr $firebird_version_req_major \+ $firebird_version_req_minor`

        AC_MSG_CHECKING([for Firebird client library >= $firebird_version_req])

        if test "$ac_firebird_path" != ""; then
            ac_firebird_ldflags="-L$ac_firebird_path/lib"
            ac_firebird_cppflags="-I$ac_firebird_path/include"
        else
            for ac_firebird_path_tmp in /usr /usr/local /opt ; do
                if test -f "$ac_firebird_path_tmp/include/$ac_firebird_header" \
                    && test -r "$ac_firebird_path_tmp/include/$ac_firebird_header"; then
                    ac_firebird_path=$ac_firebird_path_tmp
                    ac_firebird_cppflags="-I$ac_firebird_path_tmp/include"
                    ac_firebird_ldflags="-L$ac_firebird_path_tmp/lib"
                    break;
                fi
            done
        fi

        ac_firebird_header_path="$ac_firebird_path/include/$ac_firebird_header"

        if test ! -f "$ac_firebird_header_path"; then
            AC_MSG_RESULT([no])
            success="no"
        else

            ac_firebird_ldflags="$ac_firebird_ldflags -lfbclient -lpthread"

            saved_CPPFLAGS="$CPPFLAGS"
            CPPFLAGS="$CPPFLAGS $ac_firebird_cppflags"

            AC_LANG_PUSH(C++)
            AC_COMPILE_IFELSE(
                [
                AC_LANG_PROGRAM([[@%:@include <ibase.h>]],
                    [[
#if (FB_API_VER >= $firebird_version_req_number)
// Everything is okay
#else
#  error Firebird version is too old
#endif
                    ]]
                )
                ],
                [
                AC_MSG_RESULT([yes])
                success="yes"
                ],
                [
                AC_MSG_RESULT([not found])
                success="no"
                ]
            )
            AC_LANG_POP([C++])

            CPPFLAGS="$saved_CPPFLAGS"

            if test "$success" = "yes"; then

                FIREBIRD_CFLAGS="$ac_firebird_cppflags"
                FIREBIRD_LDFLAGS="$ac_firebird_ldflags"

                dnl Retrieve Firebird release version

                ac_firebird_version=`cat $ac_firebird_header_path | \
                                     grep '#define.*FB_API_VER.*' | \
                                     sed -e 's/.* //'`

                if test -n "$ac_firebird_version"; then
                    ac_firebird_version_major=`expr $ac_firebird_version \/ 10`
                    ac_firebird_version_minor=`expr $ac_firebird_version \% 10`

                    FIREBIRD_VERSION="$ac_firebird_version_major.$ac_firebird_version_minor.x"
                else
                    AC_MSG_WARN([Could not find FB_API_VER macro in $ac_firebird_header to get Firebird version.])
                fi

                AC_SUBST(FIREBIRD_CFLAGS)
                AC_SUBST(FIREBIRD_LDFLAGS)
                AC_SUBST(FIREBIRD_VERSION)
                AC_DEFINE(HAVE_FIREBIRD)
            fi
        fi
    fi
])

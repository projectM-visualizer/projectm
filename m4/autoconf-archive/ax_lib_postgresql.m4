# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_lib_postgresql.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_POSTGRESQL([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of PostgreSQL 'libpq' library
#   of particular version or newer.
#
#   AX_LIB_POSTGRESQL macro takes only one argument which is optional. If
#   there is no required version passed, then macro does not run version
#   test.
#
#   The --with-postgresql option takes one of three possible values:
#
#   no - do not check for PostgreSQL client library
#
#   yes - do check for PostgreSQL library in standard locations (pg_config
#   should be in the PATH)
#
#   path - complete path to pg_config utility, use this option if pg_config
#   can't be found in the PATH
#
#   This macro calls:
#
#     AC_SUBST(POSTGRESQL_CPPFLAGS)
#     AC_SUBST(POSTGRESQL_LDFLAGS)
#     AC_SUBST(POSTGRESQL_LIBS)
#     AC_SUBST(POSTGRESQL_VERSION)
#
#   And sets:
#
#     HAVE_POSTGRESQL
#
# LICENSE
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#   Copyright (c) 2014 Sree Harsha Totakura <sreeharsha@totakura.in>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 15

AC_DEFUN([AX_LIB_POSTGRESQL],
[
    AC_ARG_WITH([postgresql],
        AS_HELP_STRING([--with-postgresql=@<:@ARG@:>@],
            [use PostgreSQL library @<:@default=yes@:>@, optionally specify path to pg_config]
        ),
        [
        if test "$withval" = "no"; then
            want_postgresql="no"
        elif test "$withval" = "yes"; then
            want_postgresql="yes"
        else
            want_postgresql="yes"
            PG_CONFIG="$withval"
        fi
        ],
        [want_postgresql="yes"]
    )

    POSTGRESQL_CPPFLAGS=""
    POSTGRESQL_LDFLAGS=""
    POSTGRESQL_LIBS=""
    POSTGRESQL_VERSION=""

    dnl
    dnl Check PostgreSQL libraries (libpq)
    dnl

    if test "$want_postgresql" = "yes"; then

        if test -z "$PG_CONFIG" -o test; then
            AC_PATH_PROG([PG_CONFIG], [pg_config], [])
        fi

        if test ! -x "$PG_CONFIG"; then
            AC_MSG_ERROR([$PG_CONFIG does not exist or it is not an executable file])
            PG_CONFIG="no"
            found_postgresql="no"
        fi

        if test "$PG_CONFIG" != "no"; then
            AC_MSG_CHECKING([for PostgreSQL libraries])

            POSTGRESQL_CPPFLAGS="-I`$PG_CONFIG --includedir`"
            POSTGRESQL_LDFLAGS="-L`$PG_CONFIG --libdir`"
            POSTGRESQL_LIBS="-lpq"
            POSTGRESQL_VERSION=`$PG_CONFIG --version | sed -e 's#PostgreSQL ##'`

            AC_DEFINE([HAVE_POSTGRESQL], [1],
                [Define to 1 if PostgreSQL libraries are available])

            found_postgresql="yes"
            AC_MSG_RESULT([yes])
        else
            found_postgresql="no"
            AC_MSG_RESULT([no])
        fi
    fi

    dnl
    dnl Check if required version of PostgreSQL is available
    dnl


    postgresql_version_req=ifelse([$1], [], [], [$1])

    if test "$found_postgresql" = "yes" -a -n "$postgresql_version_req"; then

        AC_MSG_CHECKING([if PostgreSQL version is >= $postgresql_version_req])

        dnl Decompose required version string of PostgreSQL
        dnl and calculate its number representation
        postgresql_version_req_major=`expr $postgresql_version_req : '\([[0-9]]*\)'`
        postgresql_version_req_minor=`expr $postgresql_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        postgresql_version_req_micro=`expr $postgresql_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$postgresql_version_req_micro" = "x"; then
            postgresql_version_req_micro="0"
        fi

        postgresql_version_req_number=`expr $postgresql_version_req_major \* 1000000 \
                                   \+ $postgresql_version_req_minor \* 1000 \
                                   \+ $postgresql_version_req_micro`

        dnl Decompose version string of installed PostgreSQL
        dnl and calculate its number representation
        postgresql_version_major=`expr $POSTGRESQL_VERSION : '\([[0-9]]*\)'`
        postgresql_version_minor=`expr $POSTGRESQL_VERSION : '[[0-9]]*\.\([[0-9]]*\)'`
        postgresql_version_micro=`expr $POSTGRESQL_VERSION : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$postgresql_version_micro" = "x"; then
            postgresql_version_micro="0"
        fi

        postgresql_version_number=`expr $postgresql_version_major \* 1000000 \
                                   \+ $postgresql_version_minor \* 1000 \
                                   \+ $postgresql_version_micro`

        postgresql_version_check=`expr $postgresql_version_number \>\= $postgresql_version_req_number`
        if test "$postgresql_version_check" = "1"; then
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
            AC_DEFINE([HAVE_POSTGRESQL], [0],
                [A required version of PostgreSQL is not found])
            POSTGRESQL_CPPFLAGS=""
            POSTGRESQL_LDFLAGS=""
            POSTGRESQL_LIBS=""
        fi
    fi

    AC_SUBST([POSTGRESQL_VERSION])
    AC_SUBST([POSTGRESQL_CPPFLAGS])
    AC_SUBST([POSTGRESQL_LDFLAGS])
    AC_SUBST([POSTGRESQL_LIBS])
])

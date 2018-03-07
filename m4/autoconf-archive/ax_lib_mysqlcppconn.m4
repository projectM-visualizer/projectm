# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_lib_mysqlcppconn.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_MYSQLCPPCONN([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the MySQL Connector/C++ libraries
#
#   If no path to the installed library is given the macro searches under
#   /usr, /usr/local, /opt and /opt/local.
#
#   This macro calls:
#
#     AC_SUBST(LIBMYSQLCPPCONN_CXXFLAGS) / AC_SUBST(LIBMYSQLCPPCONN_LDFLAGS)
#
#   And sets:
#
#     HAVE_LIBMYSQLCPPCONN
#
#   Note that this library needs to link with either MySQL client library or
#   MySQL Connector/C to work. The macros defined here don't take care of
#   external dependencies so you have to do it on your own. See also
#   AX_LIB_MYSQL macro.
#
# LICENSE
#
#   Copyright (c) 2011 Xiyue Deng <manphiz@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_LIB_MYSQLCPPCONN],
[
AC_ARG_WITH([libmysqlcppconn],
    [AS_HELP_STRING([--with-libmysqlcppconn@<:@=ARG@:>@],
        [use libmysqlcppconn from a standard location (ARG=yes),
         from the specified location (ARG=<path>),
         or disable if (ARG=no)
         @<:@ARG=yes@:>@ ])],
        [
        if test "x$withval" = "xno"; then
            want_libmysqlcppconn="no"
        elif test "x$withval" = "xyes"; then
            want_libmysqlcppconn="yes"
        else
            want_libmysqlcppconn="yes"
            ac_libmysqlcppconn_path="$withval"
        fi
        ],
        [want_libmysqlcppconn="yes"])

AC_ARG_WITH([libmysqlcppconn-include],
    [AS_HELP_STRING([--with-libmysqlcppconn-include=INCLUDE_DIR],
        [Force given directory for libmysqlcppconn include path. Note that this will overwrite general library path setting.])
    ], [
        if test -d "$withval"; then
            ac_libmysqlcppconn_include_path="$withval"
        else
            AC_MSG_ERROR([--with-libmysqlcppconn-include expected directory name])
        fi
    ],
    [ac_libmysqlcppconn_include_path=""]
)

AC_ARG_WITH([libmysqlcppconn-lib],
    [AS_HELP_STRING([--with-libmysqlcppconn-lib=LIB_DIR],
        [Force given directory for libmysqlcppconn libraries. Note that this will overwrite general library path setting.])
    ], [
        if test -d "$withval"; then
            ac_libmysqlcppconn_lib_path="$withval"
        else
            AC_MSG_ERROR([--with-libmysqlcppconn-lib expected directory name])
        fi
    ],
    [ac_libmysqlcppconn_lib_path=""]
)

if test "x$want_libmysqlcppconn" = "xyes"; then
    succeeded=no

    dnl On x86_64 systems check for system libraries in both lib64 and lib.
    dnl The former is specified by FHS, but e.g. Debian does not adhere to
    dnl this (as it rises problems for generic multi-arch support).
    dnl The last entry in the list is chosen by default when no libraries
    dnl are found, e.g. when only header-only libraries are installed!
    libsubdirs="lib"
    if test `uname -m` = x86_64; then
        libsubdirs="lib lib64"
    fi

    if test "x$ac_libmysqlcppconn_path" != "x"; then
        for libsubdir in $libsubdirs ; do
            if ls "$ac_libmysqlcppconn_path/$libsubdir/libmysqlcppconn"* >/dev/null 2>&1 ; then
                LIBMYSQLCPPCONN_LDFLAGS="-L$ac_libmysqlcppconn_path/$libsubdir"
                LIBMYSQLCPPCONN_CXXFLAGS="-I$ac_libmysqlcppconn_path/include"
                break;
            fi
        done
    elif test "x$cross_compiling" != yes; then
        for ac_libmysqlcppconn_path_tmp in /usr /usr/local /opt /opt/local ; do
            for libsubdir in $libsubdirs ; do
                if ls "$ac_libmysqlcppconn_path_tmp/$libsubdir/libmysqlcppconn"* >/dev/null 2>&1 ; then
                    LIBMYSQLCPPCONN_LDFLAGS="-L$ac_libmysqlcppconn_path_tmp/$libsubdir"
                    LIBMYSQLCPPCONN_CXXFLAGS="-I$ac_libmysqlcppconn_path_tmp/include"
                    break;
                fi
            done
        done
    fi

    if test "x$ac_libmysqlcppconn_include_path" != "x" ; then
        LIBMYSQLCPPCONN_CXXFLAGS="-I$ac_libmysqlcppconn_include_path"
    fi

    if test "x$ac_libmysqlcppconn_lib_path" != "x" ; then
        LIBMYSQLCPPCONN_LDFLAGS="-L$ac_libmysqlcppconn_lib_path"
    fi

    CXXFLAGS_SAVED="$CXXFLAGS"
    CXXFLAGS="$CXXFLAGS $LIBMYSQLCPPCONN_CXXFLAGS"
    export CXXFLAGS

    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $LIBMYSQLCPPCONN_CXXFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $LIBMYSQLCPPCONN_LDFLAGS"
    export LDFLAGS


    AC_LANG_PUSH([C++])
    AC_CHECK_HEADERS([mysql_connection.h mysql_driver.h], [
            ax_cv_libmysqlcppconn_version_check=yes
        ], [
            ax_cv_libmysqlcppconn_version_check=no
        ]
    )

    AS_IF([test "x$ax_cv_libmysqlcppconn_version_check" != "xyes"], [
        AC_MSG_ERROR([Could not find working libmysqlcppconn $libmysqlcppconn_version_req installation.])
    ])

    AC_CHECK_LIB([mysqlcppconn], [main], [
    	LIBMYSQLCPPCONN_LDFLAGS="$LIBMYSQLCPPCONN_LDFLAGS -lmysqlcppconn"
        succeeded=yes
    ], [
        AC_MSG_ERROR([Could not find working libmysqlcppconn library.])
        ax_cv_libmysqlcppconn_link=no
    ])

    if test "x$succeeded" = "xyes"; then
        AC_SUBST(LIBMYSQLCPPCONN_CXXFLAGS)
        AC_SUBST(LIBMYSQLCPPCONN_LDFLAGS)
        AC_DEFINE(HAVE_LIBMYSQLCPPCONN, , [define if libmysqlcppconn is available])
        ifelse([$1], , :, [$1])
    else
        AC_MSG_NOTICE([[Could not detect the libmysqlcppconn installation. Please make sure you have a working libmysqlcppconn installation and specify its path with --with-libmysqlcppconn option.]])
        ifelse([$2], , :, [$2])
    fi

    CXXFLAGS="$CXXFLAGS_SAVED"
    export CXXFLAGS

    CPPFLAGS="$CPPFLAGS_SAVED"
    export CPPFLAGS

    LDFLAGS="$LDFLAGS_SAVED"
    export LDFLAGS

fi

])

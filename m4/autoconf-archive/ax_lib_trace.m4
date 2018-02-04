# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_lib_trace.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_TRACE([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the libtrace libraries of a particular version (or newer)
#
#   If no path to the installed library is given the macro searches under
#   /usr, /usr/local, /opt and /opt/local.
#
#   This macro calls:
#
#     AC_SUBST(LIBTRACE_CFLAGS) / AC_SUBST(LIBTRACE_LDFLAGS)
#
#   And sets:
#
#     HAVE_LIBTRACE
#
# LICENSE
#
#   Copyright (c) 2011 Xiyue Deng <manphiz@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_LIB_TRACE],
[
AC_ARG_WITH([libtrace],
    [AS_HELP_STRING([--with-libtrace@<:@=ARG@:>@],
        [use libtrace from a standard location (ARG=yes),
         from the specified location (ARG=<path>),
         or disable if (ARG=no)
         @<:@ARG=yes@:>@ ])],
        [
        if test "x$withval" = "xno"; then
            want_libtrace="no"
        elif test "x$withval" = "xyes"; then
            want_libtrace="yes"
        else
            want_libtrace="yes"
            ac_libtrace_path="$withval"
        fi
        ],
        [want_libtrace="yes"])

AC_ARG_WITH([libtrace-include],
    [AS_HELP_STRING([--with-libtrace-include=INCLUDE_DIR],
        [Force given directory for libtrace include path. Note that this will overwrite general library path setting.])
    ], [
        if test -d "$withval"; then
            ac_libtrace_include_path="$withval"
        else
            AC_MSG_ERROR([--with-libtrace-include expected directory name])
        fi
    ],
    [ac_libtrace_include_path=""]
)

AC_ARG_WITH([libtrace-lib],
    [AS_HELP_STRING([--with-libtrace-lib=LIB_DIR],
        [Force given directory for libtrace libraries. Note that this will overwrite general library path setting.])
    ], [
        if test -d "$withval"; then
            ac_libtrace_lib_path="$withval"
        else
            AC_MSG_ERROR([--with-libtrace-lib expected directory name])
        fi
    ],
    [ac_libtrace_lib_path=""]
)

if test "x$want_libtrace" = "xyes"; then
    libtrace_version_req=ifelse([$1], ,3.0.0,$1)
    libtrace_version_req_major=`expr $libtrace_version_req : '\([[0-9]]*\)'`
    libtrace_version_req_minor=`expr $libtrace_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    libtrace_version_req_macro=`expr $libtrace_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$libtrace_version_req_macro" = "x"; then
        libtrace_version_req_macro="0"
    fi

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

    if test "x$ac_libtrace_path" != "x"; then
        for libsubdir in $libsubdirs ; do
            if ls "$ac_libtrace_path/$libsubdir/libtrace"* >/dev/null 2>&1 ; then
                LIBTRACE_LDFLAGS="-L$ac_libtrace_path/$libsubdir"
                LIBTRACE_CFLAGS="-I$ac_libtrace_path/include"
                break;
            fi
        done
    elif test "x$cross_compiling" != yes; then
        for ac_libtrace_path_tmp in /usr /usr/local /opt /opt/local ; do
            for libsubdir in $libsubdirs ; do
                if ls "$ac_libtrace_path_tmp/$libsubdir/libtrace"* >/dev/null 2>&1 ; then
                    LIBTRACE_LDFLAGS="-L$ac_libtrace_path_tmp/$libsubdir"
                    LIBTRACE_CFLAGS="-I$ac_libtrace_path_tmp/include"
                    break;
                fi
            done
        done
    fi

    if test "x$ac_libtrace_include_path" != "x" ; then
        LIBTRACE_CFLAGS="-I$ac_libtrace_include_path"
    fi

    if test "x$ac_libtrace_lib_path" != "x" ; then
        LIBTRACE_LDFLAGS="-L$ac_libtrace_lib_path"
    fi

    CFLAGS_SAVED="$CFLAGS"
    CFLAGS="$CFLAGS $LIBTRACE_CFLAGS"
    export CFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $LIBTRACE_LDFLAGS"
    export LDFLAGS


    AC_CACHE_CHECK([for libtrace >= $libtrace_version_req],
        ax_cv_libtrace_version_check,
        [AC_COMPILE_IFELSE(
            [AC_LANG_PROGRAM([[
                    @%:@include <libtrace.h>
                ]], [[
                    #if !defined(LIBTRACE_API_VERSION)
                        || (LIBTRACE_API_VERSION < (($libtrace_version_req_major<<16)|($libtrace_version_req_minor<<8)|($libtrace_version_req_macro)))
                    #   error Version check failed!
                    #endif
                    return 0;
                ]]
            )], [
                ax_cv_libtrace_version_check=yes
            ], [
                ax_cv_libtrace_version_check=no
            ]
        )]
    )

    AS_IF([test "x$ax_cv_libtrace_version_check" != "xyes"], [
        AC_MSG_ERROR([Could not find working libtrace $libtrace_version_req installation.])
    ])

    AC_CHECK_LIB([trace], [main], [
    	LIBTRACE_LDFLAGS="$LIBTRACE_LDFLAGS -ltrace"
        succeeded=yes
    ], [
        AC_MSG_ERROR([Could not find working libtrace library.])
        ax_cv_libtrace_link=no
    ])

    if test "x$succeeded" = "xyes"; then
        AC_SUBST(LIBTRACE_CFLAGS)
        AC_SUBST(LIBTRACE_LDFLAGS)
        AC_DEFINE(HAVE_LIBTRACE, , [define if libtrace is available])
        ifelse([$2], , :, [$2])
    else
        AC_MSG_NOTICE([[Could not detect the libtrace installation (>= $libtrace_version_req). Please make sure you have a working libtrace installation and specify its path with --with-libtrace option.]])
        ifelse([$3], , :, [$3])
    fi

    CFLAGS="$CFLAGS_SAVED"
    export CFLAGS

    LDFLAGS="$LDFLAGS_SAVED"
    export LDFLAGS

fi

])

# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_lib_gdal.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_GDAL([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of GDAL/OGR library of
#   particular version or newer.
#
#   AX_LIB_GDAL macro takes only one argument which is optional. If there is
#   no required version passed, then macro does not run version test.
#
#   The --with-gdal option takes complete path to gdal-config utility,
#
#   This macro calls AC_SUBST for:
#
#     GDAL_VERSION
#     GDAL_CFLAGS
#     GDAL_LDFLAGS
#     GDAL_DEP_LDFLAGS
#     GDAL_OGR_ENABLED
#
#   and AC_DEFINE for:
#
#     HAVE_GDAL
#     HAVE_GDAL_OGR
#
# LICENSE
#
#   Copyright (c) 2011 Mateusz Loskot <mateusz@loskot.net>
#   Copyright (c) 2011 Alessandro Candini <candini@meeo.it>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 4

AC_DEFUN([AX_LIB_GDAL],
[
    dnl If gdal-config path is not given in ---with-gdal option,
    dnl check if it is present in the system anyway
    AC_ARG_WITH([gdal],
        AC_HELP_STRING([--with-gdal=@<:@ARG@:>@],
            [Specify full path to gdal-config script]),
        [ac_gdal_config_path=$withval],
        [gdal_config_system=check])

    dnl if gdal-config is present in the system, fill the ac_gdal_config_path variable with it full path
    AS_IF([test "x$gdal_config_system" = xcheck],
          [AC_PATH_PROG([GDAL_CONFIG], [gdal-config])],
          [AC_PATH_PROG([GDAL_CONFIG], [gdal-config],
              [no], [`dirname $ac_gdal_config_path 2> /dev/null`])]
    )

    if test ! -x "$GDAL_CONFIG"; then
        AC_MSG_ERROR([gdal-config does not exist or it is not an executable file])
            GDAL_CONFIG="no"
            found_gdal="no"
    fi

    GDAL_VERSION=""
    GDAL_CFLAGS=""
    GDAL_LDFLAGS=""
    GDAL_DEP_LDFLAGS=""
    GDAL_OGR_ENABLED=""


    dnl
    dnl Check GDAL library (libgdal)
    dnl

    if test "$GDAL_CONFIG" != "no"; then
        AC_MSG_CHECKING([for GDAL library])

        GDAL_VERSION="`$GDAL_CONFIG --version`"
        GDAL_CFLAGS="`$GDAL_CONFIG --cflags`"
        GDAL_LDFLAGS="`$GDAL_CONFIG --libs`"
        GDAL_DEP_LDFLAGS="`$GDAL_CONFIG --dep-libs`"

        AC_DEFINE([HAVE_GDAL], [1], [Define to 1 if GDAL library are available])

        found_gdal="yes"
    else
        found_gdal="no"
    fi

    AC_MSG_RESULT([$found_gdal])

    if test "$found_gdal" = "yes"; then
        AC_MSG_CHECKING([for OGR support in GDAL library])

        GDAL_OGR_ENABLED="`$GDAL_CONFIG --ogr-enabled`"
        AC_DEFINE([HAVE_GDAL_OGR], [1], [Define to 1 if GDAL library includes OGR support])

        AC_MSG_RESULT([$GDAL_OGR_ENABLED])
    fi

    dnl
    dnl Check if required version of GDAL is available
    dnl

    gdal_version_req=ifelse([$1], [], [], [$1])
    if test "$found_gdal" = "yes" -a -n "$gdal_version_req"; then

        AC_MSG_CHECKING([if GDAL version is >= $gdal_version_req])

        dnl Decompose required version string of GDAL
        dnl and calculate its number representation
        gdal_version_req_major=`expr $gdal_version_req : '\([[0-9]]*\)'`
        gdal_version_req_minor=`expr $gdal_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        gdal_version_req_micro=`expr $gdal_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$gdal_version_req_micro" = "x"; then
            gdal_version_req_micro="0"
        fi

        gdal_version_req_number=`expr $gdal_version_req_major \* 1000000 \
                                   \+ $gdal_version_req_minor \* 1000 \
                                   \+ $gdal_version_req_micro`

        dnl Decompose version string of installed GDAL
        dnl and calculate its number representation
        gdal_version_major=`expr $GDAL_VERSION : '\([[0-9]]*\)'`
        gdal_version_minor=`expr $GDAL_VERSION : '[[0-9]]*\.\([[0-9]]*\)'`
        gdal_version_micro=`expr $GDAL_VERSION : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$gdal_version_micro" = "x"; then
            gdal_version_micro="0"
        fi

        gdal_version_number=`expr $gdal_version_major \* 1000000 \
                                   \+ $gdal_version_minor \* 1000 \
                                   \+ $gdal_version_micro`

        gdal_version_check=`expr $gdal_version_number \>\= $gdal_version_req_number`
        if test "$gdal_version_check" = "1"; then
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
	    AC_MSG_ERROR([GDAL $GDAL_VERSION found, but required version is $gdal_version_req])
        fi
    fi

    AC_SUBST(GDAL_VERSION)
    AC_SUBST(GDAL_CFLAGS)
    AC_SUBST(GDAL_LDFLAGS)
    AC_SUBST(GDAL_DEP_LDFLAGS)
    AC_SUBST(GDAL_OGR_ENABLED)
])

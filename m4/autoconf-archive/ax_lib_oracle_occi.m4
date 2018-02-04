# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_lib_oracle_occi.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_ORACLE_OCCI([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of Oracle OCCI API of
#   particular version or newer. This macros checks for Oracle OCCI headers
#   and libraries and defines compilation flags.
#
#   Macro supports following options and their values:
#
#   1) Single-option usage:
#
#     --with-occi         -- path to ORACLE_HOME directory
#
#   2) Two-options usage (both options are required):
#
#     --with-occi-include -- path to directory with OCCI headers
#     --with-occi-lib     -- path to directory with OCCI libraries
#
#   NOTE: These options described above do not take yes|no values. If 'yes'
#   value is passed, then WARNING message will be displayed, 'no' value, as
#   well as the --without-occi-* variations will cause the macro to not
#   check anything.
#
#   This macro calls:
#
#     AC_SUBST(ORACLE_OCCI_CPPFLAGS)
#     AC_SUBST(ORACLE_OCCI_LDFLAGS)
#     AC_SUBST(ORACLE_OCCI_VERSION)
#
#   And sets:
#
#     HAVE_ORACLE_OCCI
#
# LICENSE
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#   Copyright (c) 2012 Krzysztof Burghardt <krzysztof@burghardt.pl>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 4

AC_DEFUN([AX_LIB_ORACLE_OCCI],
[
    AC_ARG_WITH([occi],
        AS_HELP_STRING([--with-occi=@<:@ARG@:>@],
            [use Oracle OCCI API from given Oracle home (ARG=path); use existing ORACLE_HOME (ARG=yes); disable Oracle OCCI support (ARG=no)]
        ),
        [
        if test "$withval" = "yes"; then
            if test -n "$ORACLE_HOME"; then
                oracle_home_dir="$ORACLE_HOME"
            else
                oracle_home_dir=""
            fi
        elif test -d "$withval"; then
            oracle_home_dir="$withval"
        else
            oracle_home_dir=""
        fi
        ],
        [
        if test -n "$ORACLE_HOME"; then
            oracle_home_dir="$ORACLE_HOME"
        else
            oracle_home_dir=""
        fi
        ]
    )

    AC_ARG_WITH([occi-include],
        AS_HELP_STRING([--with-occi-include=@<:@DIR@:>@],
            [use Oracle OCCI API headers from given path]
        ),
        [oracle_home_include_dir="$withval"],
        [oracle_home_include_dir=""]
    )
    AC_ARG_WITH([occi-lib],
        AS_HELP_STRING([--with-occi-lib=@<:@DIR@:>@],
            [use Oracle OCCI API libraries from given path]
        ),
        [oracle_home_lib_dir="$withval"],
        [oracle_home_lib_dir=""]
    )

    ORACLE_OCCI_CPPFLAGS=""
    ORACLE_OCCI_LDFLAGS=""
    ORACLE_OCCI_VERSION=""

    dnl
    dnl Collect include/lib paths
    dnl
    want_oracle_but_no_path="no"

    if test -n "$oracle_home_dir"; then

        if test "$oracle_home_dir" != "no" -a "$oracle_home_dir" != "yes"; then
            dnl ORACLE_HOME path provided

            dnl Primary path to OCCI headers, available in Oracle>=10
            oracle_include_dir="$oracle_home_dir/rdbms/public"

            dnl Secondary path to OCCI headers used by older versions
            oracle_include_dir2="$oracle_home_dir/rdbms/demo"

            dnl Library path
            oracle_lib_dir="$oracle_home_dir/lib"
        elif test "$oracle_home_dir" = "yes"; then
            want_oracle_but_no_path="yes"
        fi

    elif test -n "$oracle_home_include_dir" -o -n "$oracle_home_lib_dir"; then

        if test "$oracle_home_include_dir" != "no" -a "$oracle_home_include_dir" != "yes"; then
            oracle_include_dir="$oracle_home_include_dir"
        elif test "$oracle_home_include_dir" = "yes"; then
            want_oracle_but_no_path="yes"
        fi

        if test "$oracle_home_lib_dir" != "no" -a "$oracle_home_lib_dir" != "yes"; then
            oracle_lib_dir="$oracle_home_lib_dir"
        elif test "$oracle_home_lib_dir" = "yes"; then
            want_oracle_but_no_path="yes"
        fi
    fi

    if test "$want_oracle_but_no_path" = "yes"; then
        AC_MSG_WARN([Oracle support is requested but no Oracle paths have been provided. \
Please, locate Oracle directories using --with-occi or \
--with-occi-include and --with-occi-lib options.])
    fi

    dnl
    dnl Check OCCI files
    dnl
    if test -n "$oracle_include_dir" -a -n "$oracle_lib_dir"; then

        saved_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS -I$oracle_include_dir"

        dnl Additional path for older Oracle installations
        if test -n "$oracle_include_dir2"; then
            CPPFLAGS="$CPPFLAGS -I$oracle_include_dir2"
        fi

        dnl Depending on later Oracle version detection,
        dnl -lnnz11 flag might be removed for older Oracle < 10.x
        saved_LDFLAGS="$LDFLAGS"
        occi_ldflags="-L$oracle_lib_dir -lclntsh -lnnz11 -locci -lociei"
        LDFLAGS="$LDFLAGS $occi_ldflags"

        dnl
        dnl Check OCCI headers
        dnl
        AC_MSG_CHECKING([for Oracle OCCI headers in $oracle_include_dir])

        AC_LANG_PUSH(C++)
        AC_COMPILE_IFELSE([
            AC_LANG_PROGRAM([[@%:@include <occi.h>]],
                [[
#if defined(OCCI_MAJOR_VERSION)
#if OCCI_MAJOR_VERSION == 10 && OCCI_MINOR_VERSION == 2
// Oracle 10.2 detected
#endif
#elif defined(OCCI_V7_SYNTAX)
// OK, older Oracle detected
// TODO - mloskot: find better macro to check for older versions;
#else
#  error Oracle occi.h header not found
#endif
                ]]
            )],
            [
            ORACLE_OCCI_CPPFLAGS="-I$oracle_include_dir"

            if test -n "$oracle_include_dir2"; then
                ORACLE_OCCI_CPPFLAGS="$ORACLE_OCCI_CPPFLAGS -I$oracle_include_dir2"
            fi

            occi_header_found="yes"
            AC_MSG_RESULT([yes])
            ],
            [
            occi_header_found="no"
            AC_MSG_RESULT([not found])
            ]
        )
        AC_LANG_POP([C++])

        dnl
        dnl Check OCCI libraries
        dnl
        if test "$occi_header_found" = "yes"; then

            AC_MSG_CHECKING([for Oracle OCCI libraries in $oracle_lib_dir])

            AC_LANG_PUSH(C++)
            AC_LINK_IFELSE([
                AC_LANG_PROGRAM([[@%:@include <occi.h>]],
                    [[
oracle::occi::Environment *env;
env = oracle::occi::Environment::createEnvironment();
                    ]]
                )],
                [
                ORACLE_OCCI_LDFLAGS="$occi_ldflags"
                occi_lib_found="yes"
                AC_MSG_RESULT([yes])
                ],
                [
                occi_lib_found="no"
                AC_MSG_RESULT([not found])
                ]
            )
            AC_LANG_POP([C++])
        fi

        CPPFLAGS="$saved_CPPFLAGS"
        LDFLAGS="$saved_LDFLAGS"
    fi

    dnl
    dnl Check required version of Oracle is available
    dnl
    oracle_version_req=ifelse([$1], [], [], [$1])

    if test "$occi_header_found" = "yes" -a "$occi_lib_found" = "yes" -a \
        -n "$oracle_version_req"; then

        oracle_version_major=`cat $oracle_include_dir/oci.h \
                             | grep '#define.*OCI_MAJOR_VERSION.*' \
                             | sed -e 's/#define OCI_MAJOR_VERSION  *//' \
                             | sed -e 's/  *\/\*.*\*\///'`

        oracle_version_minor=`cat $oracle_include_dir/oci.h \
                             | grep '#define.*OCI_MINOR_VERSION.*' \
                             | sed -e 's/#define OCI_MINOR_VERSION  *//' \
                             | sed -e 's/  *\/\*.*\*\///'`

        AC_MSG_CHECKING([if Oracle OCCI version is >= $oracle_version_req])

        if test -n "$oracle_version_major" -a -n $"oracle_version_minor"; then

            ORACLE_OCCI_VERSION="$oracle_version_major.$oracle_version_minor"

            dnl Decompose required version string of Oracle
            dnl and calculate its number representation
            oracle_version_req_major=`expr $oracle_version_req : '\([[0-9]]*\)'`
            oracle_version_req_minor=`expr $oracle_version_req : '[[0-9]]*\.\([[0-9]]*\)'`

            oracle_version_req_number=`expr $oracle_version_req_major \* 1000000 \
                                       \+ $oracle_version_req_minor \* 1000`

            dnl Calculate its number representation
            oracle_version_number=`expr $oracle_version_major \* 1000000 \
                                  \+ $oracle_version_minor \* 1000`

            oracle_version_check=`expr $oracle_version_number \>\= $oracle_version_req_number`
            if test "$oracle_version_check" = "1"; then

                oracle_version_checked="yes"
                AC_MSG_RESULT([yes])

                dnl Add -lnnz10 flag to Oracle >= 10.x
                AC_MSG_CHECKING([for Oracle version >= 10.x and < 11.x to use -lnnz10 flag])
                oracle_nnz10_check=`expr \( $oracle_version_number \>\= 10 \* 1000000 \) \& \( $oracle_version_number \< 11 \* 1000000 \)`
                if test "$oracle_nnz10_check" = "1"; then
                    ORACLE_OCCI_LDFLAGS="$ORACLE_OCCI_LDFLAGS -lnnz10"
                    AC_MSG_RESULT([yes])
                else
                    AC_MSG_RESULT([no])
                fi

                dnl Add -lnnz11 flag to Oracle >= 11.x
                AC_MSG_CHECKING([for Oracle version >= 11.x to use -lnnz11 flag])
                oracle_nnz10_check=`expr $oracle_version_number \>\= 11 \* 1000000`
                if test "$oracle_nnz10_check" = "1"; then
                    ORACLE_OCCI_LDFLAGS="$ORACLE_OCCI_LDFLAGS -lnnz11"
                    AC_MSG_RESULT([yes])
                else
                    AC_MSG_RESULT([no])
                fi

            else
                oracle_version_checked="no"
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([Oracle $ORACLE_OCCI_VERSION found, but required version is $oracle_version_req])
            fi
        else
            ORACLE_OCCI_VERSION="UNKNOWN"
            AC_MSG_RESULT([no])
            AC_MSG_WARN([Oracle version unknown, probably OCCI older than 10.2 is available])
        fi
    fi

    AC_MSG_CHECKING([if Oracle support is enabled])

    if test "$occi_header_found" = "yes" -a "$occi_lib_found" = "yes"; then

        AC_SUBST([ORACLE_OCCI_VERSION])
        AC_SUBST([ORACLE_OCCI_CPPFLAGS])
        AC_SUBST([ORACLE_OCCI_LDFLAGS])

        HAVE_ORACLE_OCCI="yes"
    else
        HAVE_ORACLE_OCCI="no"
    fi

    AC_MSG_RESULT([$HAVE_ORACLE_OCCI])
])

# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_lib_xerces.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_XERCES([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of Apache Xerces C++ Parser of
#   particular version or newer. This macros checks for Apache Xerces C++
#   Parser headers and libraries and defines compilation flags
#
#   Macro supports following options and their values:
#
#   1) Single-option usage:
#
#     --with-xerces - yes, no or path to Xerces installation prefix
#
#   2) Three-options usage (all options are required):
#
#     --with-xerces=yes
#     --with-xerces-inc - path to base directory with Xerces headers
#     --with-xerces-lib - linker flags for Xerces
#
#   This macro calls:
#
#     AC_SUBST(XERCES_CFLAGS)
#     AC_SUBST(XERCES_LDFLAGS)
#     AC_SUBST(XERCES_VERSION) - only if version requirement is used
#
#   And sets:
#
#     HAVE_XERCES
#
# LICENSE
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 11

AC_DEFUN([AX_LIB_XERCES],
[
    AC_REQUIRE([AX_PTHREAD])

    AC_ARG_WITH([xerces],
        AS_HELP_STRING([--with-xerces=@<:@ARG@:>@],
            [use Xerces C++ Parser from given prefix (ARG=path); check standard prefixes (ARG=yes); disable (ARG=no)]
        ),
        [
        if test "$withval" = "yes"; then
            if test -d /usr/local/include/xercesc ; then
                xerces_prefix=/usr/local
            elif test -d /usr/include/xercesc ; then
                xerces_prefix=/usr
            else
                xerces_prefix=""
            fi
            xerces_requested="yes"
        elif test -d "$withval"; then
            xerces_prefix="$withval"
            xerces_requested="yes"
        else
            xerces_prefix=""
            xerces_requested="no"
        fi
        ],
        [
        dnl Default behavior is implicit yes
        if test -d /usr/local/include/xercesc ; then
            xerces_prefix=/usr/local
        elif test -d /usr/include/xercesc ; then
            xerces_prefix=/usr
        else
            xerces_prefix=""
        fi
        ]
    )

    AC_ARG_WITH([xerces-inc],
        AS_HELP_STRING([--with-xerces-inc=@<:@DIR@:>@],
            [path to Xerces C++ Parser headers]
        ),
        [xerces_include_dir="$withval"],
        [xerces_include_dir=""]
    )
    AC_ARG_WITH([xerces-lib],
        AS_HELP_STRING([--with-xerces-lib=@<:@ARG@:>@],
            [link options for Xerces C++ Parser libraries]
        ),
        [xerces_ldflags="$withval"],
        [xerces_ldflags=""]
    )

    XERCES_CPPFLAGS=""
    XERCES_LDFLAGS=""
    XERCES_VERSION=""

    dnl
    dnl Collect include/lib paths and flags
    dnl
    run_xerces_test="no"

    if test -n "$xerces_prefix"; then
        xerces_include_dir="$xerces_prefix/include"
        xerces_include_dir2="$xerces_prefix/include/xercesc"
        xerces_ldflags="-L$xerces_prefix/lib"
        run_xerces_test="yes"
    elif test "$xerces_requested" = "yes"; then
        if test -n "$xerces_include_dir" -a -n "$xerces_lib_flags"; then
            xerces_include_dir2="$xerces_include_dir/xercesc"
            run_xerces_test="yes"
        fi
    else
        run_xerces_test="no"
    fi

    xerces_libs="-lxerces-c"

    dnl
    dnl Check Xerces C++ Parser files
    dnl
    if test "$run_xerces_test" = "yes"; then

        saved_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS -I$xerces_include_dir -I$xerces_include_dir2"

        saved_LDFLAGS="$LDFLAGS"
        LDFLAGS="$LDFLAGS $xerces_ldflags $PTHREAD_LDFLAGS"

        saved_LIBS="$LIBS"
        LIBS="$xerces_libs $PTHREAD_LIBS $LIBS"

        dnl
        dnl Check Xerces headers
        dnl
        AC_MSG_CHECKING([for Xerces C++ Parser headers in $xerces_include_dir and $xerces_include_dir2])

        AC_LANG_PUSH([C++])
        AC_COMPILE_IFELSE([
            AC_LANG_PROGRAM(
                [[
@%:@include <xercesc/util/XercesDefs.hpp>
@%:@include <xercesc/util/PlatformUtils.hpp>
                ]],
                [[]]
            )],
            [
            XERCES_CPPFLAGS="-I$xerces_include_dir -I$xerces_include_dir2"
            xerces_header_found="yes"
            AC_MSG_RESULT([found])
            ],
            [
            xerces_header_found="no"
            AC_MSG_RESULT([not found])
            ]
        )
        AC_LANG_POP([C++])

        dnl
        dnl Check Xerces libraries
        dnl
        if test "$xerces_header_found" = "yes"; then

            AC_MSG_CHECKING([for Xerces C++ Parser libraries])

            AC_LANG_PUSH([C++])
            AC_LINK_IFELSE([
                AC_LANG_PROGRAM(
                    [[
@%:@include <xercesc/util/XercesDefs.hpp>
@%:@include <xercesc/util/PlatformUtils.hpp>
#if (_XERCES_VERSION >= 20200)
XERCES_CPP_NAMESPACE_USE
#endif
                    ]],
                    [[
XMLPlatformUtils::Initialize();
                    ]]
                )],
                [
                XERCES_LDFLAGS="$xerces_ldflags $PTHREAD_LDFLAGS"
                XERCES_LIBS="$xerces_libs $PTHREAD_LIBS"
                xerces_lib_found="yes"
                AC_MSG_RESULT([found])
                ],
                [
                xerces_lib_found="no"
                AC_MSG_RESULT([not found])
                ]
            )
            AC_LANG_POP([C++])
        fi

        CPPFLAGS="$saved_CPPFLAGS"
        LDFLAGS="$saved_LDFLAGS"
        LIBS="$saved_LIBS"
    fi

    AC_MSG_CHECKING([for Xerces C++ Parser])

    if test "$run_xerces_test" = "yes"; then
        if test "$xerces_header_found" = "yes" -a "$xerces_lib_found" = "yes"; then

            AC_SUBST([XERCES_CPPFLAGS])
            AC_SUBST([XERCES_LDFLAGS])
            AC_SUBST([XERCES_LIBS])

            HAVE_XERCES="yes"
        else
            HAVE_XERCES="no"
        fi

        AC_MSG_RESULT([$HAVE_XERCES])

        dnl
        dnl Check Xerces version
        dnl
        if test "$HAVE_XERCES" = "yes"; then

            xerces_version_req=ifelse([$1], [], [], [$1])

            if test  -n "$xerces_version_req"; then

                AC_MSG_CHECKING([if Xerces C++ Parser version is >= $xerces_version_req])

                if test -f "$xerces_include_dir2/util/XercesVersion.hpp"; then

                    xerces_major=`cat $xerces_include_dir2/util/XercesVersion.hpp | \
                                    grep '^#define.*XERCES_VERSION_MAJOR.*[0-9]$' | \
                                    sed -e 's/#define XERCES_VERSION_MAJOR.//'`

                    xerces_minor=`cat $xerces_include_dir2/util/XercesVersion.hpp | \
                                    grep '^#define.*XERCES_VERSION_MINOR.*[0-9]$' | \
                                    sed -e 's/#define XERCES_VERSION_MINOR.//'`

                    xerces_revision=`cat $xerces_include_dir2/util/XercesVersion.hpp | \
                                    grep '^#define.*XERCES_VERSION_REVISION.*[0-9]$' | \
                                    sed -e 's/#define XERCES_VERSION_REVISION.//'`

                    XERCES_VERSION="$xerces_major.$xerces_minor.$xerces_revision"
                    AC_SUBST([XERCES_VERSION])

                    dnl Decompose required version string and calculate numerical representation
                    xerces_version_req_major=`expr $xerces_version_req : '\([[0-9]]*\)'`
                    xerces_version_req_minor=`expr $xerces_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
                    xerces_version_req_revision=`expr $xerces_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
                    if test "x$xerces_version_req_revision" = "x"; then
                        xerces_version_req_revision="0"
                    fi

                    xerces_version_req_number=`expr $xerces_version_req_major \* 10000 \
                                               \+ $xerces_version_req_minor \* 100 \
                                               \+ $xerces_version_req_revision`

                    dnl Calculate numerical representation of detected version
                    xerces_version_number=`expr $xerces_major \* 10000 \
                                          \+ $xerces_minor \* 100 \
                                           \+ $xerces_revision`

                    xerces_version_check=`expr $xerces_version_number \>\= $xerces_version_req_number`
                    if test "$xerces_version_check" = "1"; then
                        AC_MSG_RESULT([yes])
                    else
                        AC_MSG_RESULT([no])
                        AC_MSG_WARN([Found Xerces C++ Parser $XERCES_VERSION, which is older than required. Possible compilation failure.])
                    fi
                else
                    AC_MSG_RESULT([no])
                    AC_MSG_WARN([Missing header XercesVersion.hpp. Unable to determine Xerces version.])
                fi
            fi
        fi

    else
        HAVE_XERCES="no"
        AC_MSG_RESULT([$HAVE_XERCES])

        if test "$xerces_requested" = "yes"; then
            AC_MSG_WARN([Xerces C++ Parser support requested but headers or library not found. Specify valid prefix of Xerces C++ using --with-xerces=@<:@DIR@:>@ or provide include directory and linker flags using --with-xerces-inc and --with-xerces-lib])
        fi
    fi
])

# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_lib_xalan.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_XALAN([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of Apache Xalan C++ XSLT
#   processor of a particular version or newer. This macros checks for
#   Apache Xalan C++ XSLT processor headers and libraries and defines
#   compilation flags
#
#   Macro supports following options and their values:
#
#   1) Single-option usage:
#
#     --with-xalan - yes, no or path to Xalan installation prefix
#
#   2) Three-options usage (all options are required):
#
#     --with-xalan=yes
#     --with-xalan-inc - path to base directory with Xalan headers
#     --with-xalan-lib - linker flags for Xalan
#
#   This macro calls:
#
#     AC_SUBST(XALAN_CPPFLAGS)
#     AC_SUBST(XALAN_LDFLAGS)
#     AC_SUBST(XALAN_VERSION) - only if version requirement is used
#
#   And sets:
#
#     HAVE_XALAN
#
# LICENSE
#
#   Copyright (c) 2009 Mateusz Loskot <mateusz@loskot.net>
#   Copyright (c) 2009 Bill Blough <bblough@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AC_DEFUN([AX_LIB_XALAN],
[
    AC_REQUIRE([AX_LIB_XERCES])

    AC_ARG_WITH([xalan],
        AS_HELP_STRING([--with-xalan=@<:@ARG@:>@],
            [use Xalan C++ XSLT processor from given prefix (ARG=path); check standard prefixes (ARG=yes); disable (ARG=no)]
        ),
        [
        if test "$withval" = "yes"; then
            if test -d /usr/local/include/xalanc ; then
                xalan_prefix=/usr/local
            elif test -d /usr/include/xalanc ; then
                xalan_prefix=/usr
            else
                xalan_prefix=""
            fi
            xalan_requested="yes"
        elif test -d "$withval"; then
            xalan_prefix="$withval"
            xalan_requested="yes"
        else
            xalan_prefix=""
            xalan_requested="no"
        fi
        ],
        [
        dnl Default behavior is implicit yes
        if test -d /usr/local/include/xalanc ; then
            xalan_prefix=/usr/local
        elif test -d /usr/include/xalanc ; then
            xalan_prefix=/usr
        else
            xalan_prefix=""
        fi
        ]
    )

    AC_ARG_WITH([xalan-inc],
        AS_HELP_STRING([--with-xalan-inc=@<:@DIR@:>@],
            [path to Xalan C++ XSLT processor headers]
        ),
        [xalan_include_dir="$withval"],
        [xalan_include_dir=""]
    )
    AC_ARG_WITH([xalan-lib],
        AS_HELP_STRING([--with-xalan-lib=@<:@ARG@:>@],
            [link options for Xalan C++ XSLT processor libraries]
        ),
        [xalan_ldflags="$withval"],
        [xalan_ldflags=""]
    )

    XALAN_CPPFLAGS=""
    XALAN_LDFLAGS=""
    XALAN_VERSION=""

    dnl
    dnl Collect include/lib paths and flags
    dnl
    run_xalan_test="no"

    if test -n "$xalan_prefix"; then
        xalan_include_dir="$xalan_prefix/include"
        xalan_include_dir2="$xalan_prefix/include/xalanc"
        xalan_ldflags="-L$xalan_prefix/lib"
        run_xalan_test="yes"
    elif test "$xalan_requested" = "yes"; then
        if test -n "$xalan_include_dir" -a -n "$xalan_lib_flags"; then
            xalan_include_dir2="$xalan_include_dir/xalanc"
            run_xalan_test="yes"
        fi
    else
        run_xalan_test="no"
    fi

    xalan_libs="-lxalan-c"

    dnl
    dnl Check Xalan C++ XSLT processor files
    dnl
    if test "$run_xalan_test" = "yes"; then

        saved_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS -I$xalan_include_dir -I$xalan_include_dir2"

        saved_LDFLAGS="$LDFLAGS"
        LDFLAGS="$LDFLAGS $xalan_ldflags $XERCES_LDFLAGS"

        saved_LIBS="$LIBS"
        LIBS="$xalan_libs $XERCES_LIBS $LIBS"

        dnl
        dnl Check Xalan headers
        dnl
        AC_MSG_CHECKING([for Xalan C++ XSLT processor headers in $xalan_include_dir and $xalan_include_dir2])

        AC_LANG_PUSH([C++])
        AC_COMPILE_IFELSE([
            AC_LANG_PROGRAM(
                [[
@%:@include <xalanc/Include/XalanVersion.hpp>
@%:@include <xalanc/PlatformSupport/PlatformSupportInit.hpp>
                ]],
                [[]]
            )],
            [
            XALAN_CPPFLAGS="-I$xalan_include_dir -I$xalan_include_dir2"
            xalan_header_found="yes"
            AC_MSG_RESULT([found])
            ],
            [
            xalan_header_found="no"
            AC_MSG_RESULT([not found])
            ]
        )
        AC_LANG_POP([C++])

        dnl
        dnl Check Xalan libraries
        dnl
        if test "$xalan_header_found" = "yes"; then

            AC_MSG_CHECKING([for Xalan C++ XSLT processor libraries])

            AC_LANG_PUSH([C++])
            AC_LINK_IFELSE([
                AC_LANG_PROGRAM(
                    [[
@%:@include <xalanc/PlatformSupport/PlatformSupportInit.hpp>
@%:@include <xercesc/util/PlatformUtils.hpp>
@%:@include <xalanc/XalanTransformer/XalanTransformer.hpp>
XALAN_USING_XERCES(XMLPlatformUtils)
XALAN_USING_XALAN(XalanTransformer)
                    ]],
                    [[
XMLPlatformUtils::Initialize();
XalanTransformer::initialize();

                    ]]
                )],
                [
                XALAN_LDFLAGS="$xalan_ldflags $XERCES_LDFLAGS"
                XALAN_LIBS="$xalan_libs $XERCES_LIBS"
                xalan_lib_found="yes"
                AC_MSG_RESULT([found])
                ],
                [
                xalan_lib_found="no"
                AC_MSG_RESULT([not found])
                ]
            )
            AC_LANG_POP([C++])
        fi

        CPPFLAGS="$saved_CPPFLAGS"
        LDFLAGS="$saved_LDFLAGS"
        LIBS="$saved_LIBS"
    fi

    AC_MSG_CHECKING([for Xalan C++ XSLT processor])

    if test "$run_xalan_test" = "yes"; then
        if test "$xalan_header_found" = "yes" -a "$xalan_lib_found" = "yes"; then

            AC_SUBST([XALAN_CPPFLAGS])
            AC_SUBST([XALAN_LDFLAGS])
            AC_SUBST([XALAN_LIBS])

            HAVE_XALAN="yes"
        else
            HAVE_XALAN="no"
        fi

        AC_MSG_RESULT([$HAVE_XALAN])

        dnl
        dnl Check Xalan version
        dnl
        if test "$HAVE_XALAN" = "yes"; then

            xalan_version_req=ifelse([$1], [], [], [$1])

            if test  -n "$xalan_version_req"; then

                AC_MSG_CHECKING([if Xalan C++ XSLT processor version is >= $xalan_version_req])

                if test -f "$xalan_include_dir2/Include/XalanVersion.hpp"; then

                    xalan_major=`cat $xalan_include_dir2/Include/XalanVersion.hpp | \
                                    grep '^#define.*XALAN_VERSION_MAJOR.*[0-9]$' | \
                                    sed -e 's/#define XALAN_VERSION_MAJOR.//'`

                    xalan_minor=`cat $xalan_include_dir2/Include/XalanVersion.hpp | \
                                    grep '^#define.*XALAN_VERSION_MINOR.*[0-9]$' | \
                                    sed -e 's/#define XALAN_VERSION_MINOR.//'`

                    xalan_revision=`cat $xalan_include_dir2/Include/XalanVersion.hpp | \
                                    grep '^#define.*XALAN_VERSION_REVISION.*[0-9]$' | \
                                    sed -e 's/#define XALAN_VERSION_REVISION.//'`

                    XALAN_VERSION="$xalan_major.$xalan_minor.$xalan_revision"
                    AC_SUBST([XALAN_VERSION])

                    dnl Decompose required version string and calculate numerical representation
                    xalan_version_req_major=`expr $xalan_version_req : '\([[0-9]]*\)'`
                    xalan_version_req_minor=`expr $xalan_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
                    xalan_version_req_revision=`expr $xalan_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
                    if test "x$xalan_version_req_revision" = "x"; then
                        xalan_version_req_revision="0"
                    fi

                    xalan_version_req_number=`expr $xalan_version_req_major \* 10000 \
                                               \+ $xalan_version_req_minor \* 100 \
                                               \+ $xalan_version_req_revision`

                    dnl Calculate numerical representation of detected version
                    xalan_version_number=`expr $xalan_major \* 10000 \
                                          \+ $xalan_minor \* 100 \
                                          \+ $xalan_revision`

                    xalan_version_check=`expr $xalan_version_number \>\= $xalan_version_req_number`
                    if test "$xalan_version_check" = "1"; then
                        AC_MSG_RESULT([yes])
                    else
                        AC_MSG_RESULT([no])
                        AC_MSG_WARN([Found Xalan C++ XSLT processor $XALAN_VERSION, which is older than required. Possible compilation failure.])
                    fi
                else
                    AC_MSG_RESULT([no])
                    AC_MSG_WARN([Missing header XalanVersion.hpp. Unable to determine Xalan version.])
                fi
            fi
        fi

    else
        HAVE_XALAN="no"
        AC_MSG_RESULT([$HAVE_XALAN])

        if test "$xalan_requested" = "yes"; then
            AC_MSG_WARN([Xalan C++ XSLT processor support requested but headers or library not found. Specify valid prefix of Xalan C++ using --with-xalan=@<:@DIR@:>@ or provide include directory and linker flags using --with-xalan-inc and --with-xalan-lib])
        fi
    fi
])

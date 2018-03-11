# ===========================================================================
#        https://www.gnu.org/software/autoconf-archive/ax_xsdcxx.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_XSDCXX
#
# DESCRIPTION
#
#   This macro tries to find the CodeSynthesis XSD executable and the
#   corresponding headers.
#
#   Supported options:
#
#     --with-xsdcxx
#         use CodeSynthesis XSD from given prefix (ARG=path);
#         check PATH (ARG=yes); disable (ARG=no)
#
#     --with-xsdcxx-inc
#         path to CodeSynthesis XSD headers (ARG=path); use standard prefix
#         (ARG=yes); disable (ARG=no)
#
#   Output variables:
#
#     XSDCXX
#     XSDCXX_CPPFLAGS
#
# LICENSE
#
#   Copyright (c) 2013 Laszlo Kajan <lkajan@rostlab.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_XSDCXX],
[
    AC_ARG_WITH([xsdcxx],
        [AS_HELP_STRING([--with-xsdcxx=@<:@ARG@:>@],
            [use CodeSynthesis XSD from given prefix (ARG=path); check PATH (ARG=yes); disable (ARG=no)])],
        [],
        [with_xsdcxx=yes])

    AS_IF([test "x$with_xsdcxx" != xno],
        [if test "$with_xsdcxx" = "yes"; then with_xsdcxx=xsdcxx; fi
        AC_CHECK_PROG([XSDCXX], [$with_xsdcxx], [$with_xsdcxx])
        if test "x$XSDCXX" = x; then
            AC_CHECK_PROG([XSDCXX], [xsdcxx], [xsdcxx])
            if test "x$XSDCXX" = x; then
                AC_CHECK_PROG([XSDCXX], [xsd], [xsd])
            fi
        fi])

    AC_ARG_WITH([xsdcxx-inc],
        AS_HELP_STRING([--with-xsdcxx-inc=@<:@DIR@:>@],
            [path to CodeSynthesis XSD headers (ARG=path); use standard prefix (ARG=); disable (ARG=no)]
        ),
        [xsdcxx_include_dir="$withval"],
        [xsdcxx_include_dir=]
    )

    XSDCXX_CPPFLAGS=""
    if test "x$xsdcxx_include_dir" != "xno" -a "x$xsdcxx_include_dir" != "x"; then
            AC_SUBST([XSDCXX_CPPFLAGS],[-I$xsdcxx_include_dir])
    fi
])
# vim:et:

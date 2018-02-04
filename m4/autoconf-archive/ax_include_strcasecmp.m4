# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_include_strcasecmp.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_INCLUDE_STRCASECMP
#
# DESCRIPTION
#
#   This macro tries to find a header for strcasecmp() in strings.h, then
#   string.h, and AC_DEFINEs AX_STRCASECMP_HEADER to the value it found. Use
#   it in your source like so:
#
#     #ifdef AX_STRCASECMP_HEADER
#     #include AX_STRCASECMP_HEADER
#     #endif
#
# LICENSE
#
#   Copyright (c) 2012 Leo Davis <ldavis@speechfxinc.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_INCLUDE_STRCASECMP],
[
AC_CACHE_CHECK([for strcasecmp header file], [ax_cv_include_strcasecmp_found], [
        ax_cv_include_strcasecmp_found=no
        AC_TRY_LINK(
                [ #include <strings.h> ],
                [ strcasecmp("foo", "bar"); ],
                [ax_cv_include_strcasecmp_found='<strings.h>'],
                [ax_cv_include_strcasecmp_found=no])

        if test x"$ax_cv_include_strcasecmp_found" = "xno"
        then
        AC_TRY_LINK(
                [ #include <string.h> ],
                [ strcasecmp("foo", "bar"); ],
                [ax_cv_include_strcasecmp_found='<string.h>'],
                [ax_cv_include_strcasecmp_found=no])
        fi
])
        if test x"$ax_cv_include_strcasecmp_found" != "xno"
        then
            AC_DEFINE_UNQUOTED([AX_STRCASECMP_HEADER], [$ax_cv_include_strcasecmp_found],
                                                       [Defined to <strings.h> or <string.h> if strcasecmp is found])

        fi
]) dnl AX_INCLUDE_STRCASECMP

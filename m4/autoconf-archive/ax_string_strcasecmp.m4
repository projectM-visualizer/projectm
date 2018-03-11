# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_string_strcasecmp.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_STRING_STRCASECMP
#
# DESCRIPTION
#
#   This macro tries to find strcasecmp() in string.h.
#
#   Use this macro in conjunction with AX_STRINGS_STRCASECMP in your
#   configure.in like so:
#
#     AX_STRING_STRCASECMP
#     if test x"$ac_cv_string_strcasecmp" = "xno" ; then
#         AX_STRINGS_STRCASECMP
#     fi
#
#   This will cause either HAVE_STRING_STRCASECMP or HAVE_STRINGS_STRCASECMP
#   to be defined in config.h, which will tell your code what header to
#   include to get strcasecmp()'s prototype.
#
# LICENSE
#
#   Copyright (c) 2008 Warren Young <warren@etr-usa.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AU_ALIAS([ETR_STRING_STRCASECMP], [AX_STRING_STRCASECMP])
AC_DEFUN([AX_STRING_STRCASECMP],
[
AC_CACHE_CHECK([for strcasecmp() in string.h], ac_cv_string_strcasecmp, [
        AC_TRY_LINK(
                [ #include <string.h> ],
                [ strcasecmp("foo", "bar"); ],
                ac_cv_string_strcasecmp=yes,
                ac_cv_string_strcasecmp=no)
])

        if test x"$ac_cv_string_strcasecmp" = "xyes"
        then
                AC_DEFINE(HAVE_STRING_STRCASECMP, 1,
                        [ Define if your system has strcasecmp() in string.h ])
        fi
]) dnl AX_STRING_STRCASECMP

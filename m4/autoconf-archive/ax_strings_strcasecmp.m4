# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_strings_strcasecmp.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_STRINGS_STRCASECMP
#
# DESCRIPTION
#
#   This macro tries to find strcasecmp() in strings.h. See the
#   AX_STRING_STRCASECMP macro's commentary for usage details.
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

AU_ALIAS([ETR_STRINGS_STRCASECMP], [AX_STRINGS_STRCASECMP])
AC_DEFUN([AX_STRINGS_STRCASECMP],
[ AC_CACHE_CHECK([for strcasecmp() in strings.h], ac_cv_strings_strcasecmp, [

        AC_TRY_LINK(
                [ #include <strings.h> ],
                [ strcasecmp("foo", "bar"); ],
                ac_cv_strings_strcasecmp=yes,
                ac_cv_strings_strcasecmp=no)
])

        if test x"$ac_cv_strings_strcasecmp" = "xyes"
        then
                AC_DEFINE(HAVE_STRINGS_STRCASECMP, 1,
                        [ Define if your system has strcasecmp() in strings.h ])
        fi
]) dnl AX_STRINGS_STRCASECMP

# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_with_apxs.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_WITH_APXS([value-if-not-found], [path])
#
# DESCRIPTION
#
#   Locates an installed apxs binary, placing the result in the precious
#   variable $APXS. Accepts a preset $APXS, then --with-apxs, and failing
#   that searches for apxs in the given path (which defaults to the system
#   path). If apxs is found, $APXS is set to the full path of the binary;
#   otherwise it is set to VALUE-IF-NOT-FOUND, which defaults to apxs.
#
#   Example:
#
#     AX_WITH_APXS(missing)
#
# LICENSE
#
#   Copyright (c) 2008 Dustin J. Mitchell <dustin@cs.uchicago.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 11

AC_DEFUN([AX_WITH_APXS],
[
  AC_ARG_VAR([APXS])

  dnl unless APXS was supplied to us (as a precious variable)
  if test -z "$APXS"
  then
    AC_MSG_CHECKING(for --with-apxs)
    AC_ARG_WITH(apxs,
                AS_HELP_STRING([--with-apxs=APXS],
                               [absolute path name of apxs executable]),
                [ if test "$withval" != "yes"
                  then
                    APXS="$withval"
                    AC_MSG_RESULT($withval)
                  else
                    AC_MSG_RESULT(no)
                  fi
                ],
                [ AC_MSG_RESULT(no)
                ])
  fi

  dnl if it's still not found, check the paths, or use the fallback
  if test -z "$APXS"
  then
    AC_PATH_PROG([APXS], apxs, m4_ifval([$1],[$1],[apxs]), $2)
  fi
])

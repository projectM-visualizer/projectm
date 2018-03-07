# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_need_awk.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_NEED_AWK
#
# DESCRIPTION
#
#   Check if an awk implementation is available. Bail-out if not found.
#
#   This work is heavily based upon testawk.sh script by Heiner Steven. You
#   should find his script (and related works) at
#   <http://www.shelldorado.com/articles/awkcompat.html>. Thanks to
#   Alessandro Massignan for his suggestions and extensive nawk tests on
#   FreeBSD.
#
# LICENSE
#
#   Copyright (c) 2009 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 9

AC_DEFUN([AX_NEED_AWK],[
  AC_REQUIRE([AC_PROG_AWK])

  AS_IF([test "x$AWK" = "x"],[
    AC_MSG_ERROR([cannot find awk, bailing out])
  ])
])

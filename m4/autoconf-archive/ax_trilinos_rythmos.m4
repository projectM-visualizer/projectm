# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_trilinos_rythmos.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_TRILINOS_RYTHMOS([, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   On success, adds "include Makefile.export.rythmos" statements to every
#   Automake file containing @INC_AMINCLUDE@. Requires that Trilinos was
#   configured with the --enable-export-makefiles option. When
#   ACTION-IF-NOT-FOUND is not specified, the default behavior is for
#   configure to fail.
#
# LICENSE
#
#   Copyright (c) 2009 Rhys Ulerich <rhys.ulerich@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_TRILINOS_RYTHMOS],[
    AC_REQUIRE([AX_TRILINOS_BASE])
    ax_trilinos_rythmos=yes
    AC_HAVE_LIBRARY([rythmos],[:],[ax_trilinos_rythmos=no])
    AX_ADD_AM_TRILINOS_MAKEFILE_EXPORT([rythmos.macros],[ax_trilinos_rythmos=no])
    AX_ADD_AM_TRILINOS_MAKEFILE_EXPORT([rythmos],[ax_trilinos_rythmos=no])
    if test "$ax_trilinos_rythmos" = yes; then
        : # NOP
		ifelse([$1],,,
            [$1])
    else
        : # NOP
		ifelse([$2],,AC_MSG_ERROR([Trilinos Rythmos not usable.]),
            [$2])
    fi
])

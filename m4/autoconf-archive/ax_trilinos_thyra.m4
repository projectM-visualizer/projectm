# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_trilinos_thyra.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_TRILINOS_THYRA([, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   On success, adds "include Makefile.export.thyra" statements to every
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

AC_DEFUN([AX_TRILINOS_THYRA],[
    AC_REQUIRE([AX_TRILINOS_BASE])
    ax_trilinos_thyra=yes
    AC_HAVE_LIBRARY([thyra],[:],[ax_trilinos_thyra=no])
    AX_ADD_AM_TRILINOS_MAKEFILE_EXPORT([thyra.macros],[ax_trilinos_thyra=no])
    AX_ADD_AM_TRILINOS_MAKEFILE_EXPORT([thyra],[ax_trilinos_thyra=no])
    if test "$ax_trilinos_thyra" = yes; then
        : # NOP
		ifelse([$1],,,
            [$1])
    else
        : # NOP
		ifelse([$2],,AC_MSG_ERROR([Trilinos Thyra not usable.]),
            [$2])
    fi
])

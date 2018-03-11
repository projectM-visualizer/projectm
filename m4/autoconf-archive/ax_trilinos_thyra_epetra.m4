# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_trilinos_thyra_epetra.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_TRILINOS_THYRA_EPETRA([, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Checks if the Thyra/Epetra adapters were compiled with Trilinos. When
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

AC_DEFUN([AX_TRILINOS_THYRA_EPETRA],[
    AC_REQUIRE([AX_TRILINOS_BASE])
    AC_REQUIRE([AX_TRILINOS_EPETRA])
    AC_REQUIRE([AX_TRILINOS_THYRA])
    ax_trilinos_thyra_epetra=yes
    AC_HAVE_LIBRARY([thyraepetra],[:],[ax_trilinos_thyra_epetra=no])
    if test "$ax_trilinos_thyra_epetra" = yes; then
        : # NOP
		ifelse([$1],,,
            [$1])
    else
        : # NOP
		ifelse([$2],,AC_MSG_ERROR([Trilinos Thyra/Epetra support not usable.]),
            [$2])
    fi
])

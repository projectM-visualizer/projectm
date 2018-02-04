# ================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_trilinos_thyra_epetraext.html
# ================================================================================
#
# SYNOPSIS
#
#   AX_TRILINOS_THYRA_EPETRAEXT([, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Checks if the Thyra/EpetraExt adapters were compiled with Trilinos. When
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

AC_DEFUN([AX_TRILINOS_THYRA_EPETRAEXT],[
    AC_REQUIRE([AX_TRILINOS_BASE])
    AC_REQUIRE([AX_TRILINOS_EPETRAEXT])
    AC_REQUIRE([AX_TRILINOS_THYRA_EPETRA])
    ax_trilinos_thyra_epetraext=yes
    AC_HAVE_LIBRARY([thyraepetraext],[:],[ax_trilinos_thyra_epetraext=no])
    if test "$ax_trilinos_thyra_epetraext" = yes; then
        : # NOP
		ifelse([$1],,,
            [$1])
    else
        : # NOP
		ifelse([$2],,AC_MSG_ERROR([Trilinos Thyra/EpetraExt adapters not usable.]),
            [$2])
    fi
])

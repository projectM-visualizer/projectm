# ===============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_trilinos_epetraext_hdf5.html
# ===============================================================================
#
# SYNOPSIS
#
#   AX_TRILINOS_EPETRAEXT_HDF5([, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   Checks if EpetraExt was compiled with HDF5 support
#   (--enable-epetraext-hdf5) enabled. When ACTION-IF-NOT-FOUND is not
#   specified, the default behavior is for configure to fail.
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

AC_DEFUN([AX_TRILINOS_EPETRAEXT_HDF5],[
    AC_REQUIRE([AC_LIB_HDF5])
    AC_REQUIRE([AX_TRILINOS_EPETRAEXT])
    ax_trilinos_epetraext_hdf5=yes
    AC_CHECK_HEADER([EpetraExt_ConfigDefs.h],,[ax_trilinos_epetraext_hdf5=no])
    AC_MSG_CHECKING([for HDF5 support in EpetraExt release])
    AC_LANG_PUSH(C++)
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        @%:@include <EpetraExt_ConfigDefs.h>
        ]], [[
        #ifndef HAVE_EPETRAEXT_HDF5
        #  error HAVE_EPETRAEXT_HDF5 was not defined in EpetraExt_ConfigDefs.h
        #endif
    ]])],[
        AC_MSG_RESULT(yes)
    ],[
        AC_MSG_RESULT(no)
        ax_trilinos_epetraext_hdf5=no
    ])
    AC_LANG_POP([C++])
    if test "$ax_trilinos_epetraext_hdf5" = yes; then
        : # NOP
		ifelse([$1],,,
            [$1])
    else
        : # NOP
		ifelse([$2],,AC_MSG_ERROR([Trilinos EpetraExt HDF5 not usable.]),
            [$2])
    fi
])

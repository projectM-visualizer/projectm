# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_path_missing.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PATH_MISSING(VARIABLE, PROG-TO-CHECK-FOR, WARNING-IF-NOT-FOUND, PATH)
#
# DESCRIPTION
#
#   Check whether program PROG-TO-CHECK-FOR exists in path. If it is found,
#   set VARIABLE to the full path of PROG-TO-CHECK-FOR, otherwise warn using
#   the string WARNING-IF-NOT-FOUND and set VARIABLE to the full path of the
#   Automake missing script with PROG-TO-CHECK-FOR as the command to run.
#
#   A typical use is the following:
#
#     AX_PATH_MISSING([AUTOGEN],[autogen],[autogen seems missing ...])
#
#   This macro is the combination of AC_PATH_PROG and AX_MISSING_PROG. If
#   you do not want to run AC_PATH_PROG, simply use AX_MISSING_PROG or
#   AM_MISSING.
#
# LICENSE
#
#   Copyright (c) 2008 Noah Slater <nslater@bytesexual.org>
#   Copyright (c) 2008 Francesco Salvestrini <salvestrini@sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 12

AC_DEFUN([AX_PATH_MISSING], [
    AC_PATH_PROG([$1],[$2],[$4])
    AS_IF([ test -z "${$1}" ],[
        unset $1
        AX_MISSING_PROG([$1],[$2],[$3])
    ])
])

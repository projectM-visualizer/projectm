# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_lib_taglib.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_TAGLIB([VERSION],[ACTION-IF-SUCCESS],[ACTION-IF-FAILURE])
#
# DESCRIPTION
#
#   Checks for minimum taglib library version VERSION. If successful
#   executes ACTION-IF-SUCCESS otherwise ACTION-IF-FAILURE.
#
#   Defines TAGLIB_LIBS and TAGLIB_CFLAGS.
#
#   A simple example:
#
#     AX_LIB_TAGLIB([1.5],,[
#       AC_MSG_ERROR([Your system lacks of taglib >= 1.5])
#     ])
#
#   This macro is a rearranged version of AC_LIB_TAGLIB from Akos Maroy.
#
# LICENSE
#
#   Copyright (c) 2009 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AU_ALIAS([AC_CHECK_TAGLIB], [AX_LIB_TAGLIB])
AC_DEFUN([AX_LIB_TAGLIB], [
  AX_PATH_GENERIC([taglib],[$1],,[$2],[$3])
])

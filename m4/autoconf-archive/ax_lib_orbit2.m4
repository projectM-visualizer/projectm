# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_lib_orbit2.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_ORBIT2([VERSION],[ACTION-IF-SUCCESS],[ACTION-IF-FAILURE])
#
# DESCRIPTION
#
#   Checks for minimum Orbit2 library version VERSION. If successful
#   executes ACTION-IF-SUCCESS otherwise ACTION-IF-FAILURE.
#
#   Defines ORBIT2_LIBS and ORBIT2_CFLAGS.
#
#   A simple example:
#
#     AX_LIB_ORBIT2([2.14.16],,[
#       AC_MSG_ERROR([Your system lacks of orbit2 >= 2.14.16])
#     ])
#
# LICENSE
#
#   Copyright (c) 2009 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_LIB_ORBIT2], [
  AX_PATH_GENERIC([orbit2],[$1],'s/^ORBit2\ \+//',[$2],[$3])
])

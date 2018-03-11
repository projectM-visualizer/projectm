# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_prog_md5sum.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_MD5SUM([ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro find a md5sum application and set the variable md5sum to the
#   name of the application or to no if not found if ACTION-IF-NOT-FOUND is
#   not specified, configure fail when then application is not found.
#
# LICENSE
#
#   Copyright (c) 2009 Gabriele Bartolini <gabriele.bartolini@devise.it>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_PROG_MD5SUM],[
# Mac users have md5 instead of md5sum
AC_CHECK_PROGS(md5sum,[md5sum md5],no)
if test $md5sum = "no" ;
then
       ifelse($#,0,[AC_MSG_ERROR([Unable to find the md5sum application (or equivalent)])],
               $1)
fi
])

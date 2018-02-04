# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_prog_scalac.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_SCALAC
#
# DESCRIPTION
#
#   AX_PROG_SCALAC will check that a scala compiler is available, that it
#   works and set SCALAC for the Makefile.
#
#   AX_PROG_SCALAC will search the path for a Scala compiler. To choose
#   another compiler use the environment variable SCALAC.
#
# LICENSE
#
#   Copyright (c) 2013 Sveinung Kvilhaugsvik <sveinung84@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 2

AC_ARG_VAR(SCALAC, [scala compiler])

AC_DEFUN([AX_PROG_SCALAC], [
  AC_CHECK_PROGS(SCALAC, [scalac])
  AS_IF([test x"$SCALAC" != x], [
    AC_CACHE_VAL(ax_cv_scalac_works, [
      AC_MSG_CHECKING([if the Scala compiler works])

      TESTFILE=conftest.scala
      echo ["object conftest {"] > $TESTFILE
      echo ["  def main(args: Array[String]): Unit = return"] >> $TESTFILE
      echo ["}"] >> $TESTFILE

      [$SCALAC $TESTFILE && ax_cv_scalac_works=yes || ax_cv_scalac_works=no]
      AC_MSG_RESULT($ax_cv_scalac_works)

      AS_IF([test x"$ax_cv_scalac_works" != xyes], [
        AC_MSG_ERROR([Scala compiler does not work. Unable to compile $TESTFILE])
      ])
    ])
  ], AC_MSG_WARN([Scala compiler not found]))
])

# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_prog_scala.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_SCALA
#
# DESCRIPTION
#
#   AX_PROG_SCALA will check that scala is available. If it is SCALA will be
#   set to its path and SCALA_VERSION will be set to its version.
#
#   Hint: AX_COMPARE_VERSION can be used to check if SCALA_VERSION has an
#   acceptable value.
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

AC_ARG_VAR(SCALA, [scala runner])

AC_DEFUN([AX_PROG_SCALA], [
  AC_CHECK_PROGS(SCALA, [scala])
  AS_IF([test x"$SCALA" != x], [
      AC_SUBST(SCALA_VERSION, `"$SCALA" -e "println(util.Properties.versionString.split(\" \")(1))"`)
      AC_MSG_NOTICE([Scala version $SCALA_VERSION found])
    ], AC_MSG_WARN([Scala not found]))])

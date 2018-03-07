# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_find_scala_stdlib.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_FIND_SCALA_STDLIB
#
# DESCRIPTION
#
#   AX_FIND_SCALA_STDLIB will look for the Scala Standard Library. When
#   found its path will be put in SCALA_STDLIB_CPF as a classpath fragment.
#
#   To override the location define SCALA_STDLIB to the wanted path
#
# LICENSE
#
#   Copyright (c) 2013 Sveinung Kvilhaugsvik <sveinung84@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification,are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 2

AC_ARG_VAR(SCALA_STDLIB, [path to the Scala Standard Library])

AC_DEFUN([AX_FIND_SCALA_STDLIB], [
  AC_CACHE_VAL(ax_cv_scala_libray, [
    AS_IF([test x"$SCALA_STDLIB" != x],
      [ax_cv_scala_stdlib_cpf="$SCALA_STDLIB$PATH_SEPARATOR"],
      [AX_XTRA_CLASSPATH(ax_cv_scala_stdlib_cpf,
        scala.collection.immutable.List, scala-library.jar)])])

  AS_IF([test x"$ax_cv_scala_stdlib_cpf" != x], [
    AC_MSG_NOTICE([Scala Standard Library found $ax_cv_scala_stdlib_cpf])
    AC_SUBST(SCALA_STDLIB_CPF, $ax_cv_scala_stdlib_cpf)
  ], AC_MSG_WARN([Scala Standard Library not found]))])

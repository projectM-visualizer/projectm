# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_find_hamcrest.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_FIND_HAMCREST
#
# DESCRIPTION
#
#   AX_FIND_HAMCREST will look for the Hamcrest java library. When found its
#   path will be put in HAMCREST_CPF as a classpath fragment.
#
#   To override the location define HAMCREST_JAR to the wanted path.
#
# LICENSE
#
#   Copyright (c) 2013 Sveinung Kvilhaugsvik <sveinung84@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 3

AC_ARG_VAR(HAMCREST_JAR,
  [the location of the Hamcrest Matcher java library])

AC_DEFUN([AX_FIND_HAMCREST], [
  AC_CACHE_VAL(ax_cv_hamcrest_cpf, [
    AS_IF([test x"$HAMCREST_JAR" != x],
      [ax_cv_hamcrest_cpf="$HAMCREST_JAR$PATH_SEPARATOR"], [
        AX_XTRA_CLASSPATH(ax_cv_hamcrest_cpf, org.hamcrest.Matcher,
          [hamcrest-core-1.3.jar hamcrest-core.jar])])])

  AS_IF([test x"$ax_cv_hamcrest_cpf" != x],
    [AC_SUBST(HAMCREST_CPF, $ax_cv_hamcrest_cpf)],
    AC_MSG_WARN([Hamcrest Matcher not found]))])

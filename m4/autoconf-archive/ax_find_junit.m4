# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_find_junit.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_FIND_JUNIT
#
# DESCRIPTION
#
#   AX_FIND_JUNIT will look for the JUnit java library. When found its path
#   will be put in JUNIT_CPF as a classpath fragment.
#
#   To override the location define JUNIT_JAR to the wanted path.
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

AC_ARG_VAR(JUNIT_JAR, [the location of JUnit 4])

AC_DEFUN([AX_FIND_JUNIT], [
  AC_CACHE_VAL(ax_cv_junit_cpf, [
    AS_IF([test x"$JUNIT_JAR" != x],
      [ax_cv_junit_cpf="$JUNIT_JAR$PATH_SEPARATOR"], [
        AX_XTRA_CLASSPATH(ax_cv_junit_cpf, org.junit.Assert,
          [junit4-4.11.jar junit4.jar junit.jar])

        # junit.jar may not have org.hamcrest.Matcher
        classpath_when_starting_ax_find_junit="$CLASSPATH"
        CLASSPATH="$ax_cv_junit_cpf$classpath_when_starting_ax_find_junit"
        AC_CHECK_CLASS(org.hamcrest.Matcher,
          built_in_hamcrest=yes, built_in_hamcrest=no)
        CLASSPATH="$classpath_when_starting_ax_find_junit"

        AS_IF([test x"$built_in_hamcrest" = xno], [
          AX_FIND_HAMCREST
          ax_cv_junit_cpf="$HAMCREST_CPF$ax_cv_junit_cpf"])])])

  AS_IF([test x"$ax_cv_junit_cpf" != x], [
      AC_MSG_NOTICE([JUnit 4 found $ax_cv_junit_cpf])
      AC_SUBST(JUNIT_CPF, $ax_cv_junit_cpf)],
    AC_MSG_WARN([JUnit 4 not found]))])

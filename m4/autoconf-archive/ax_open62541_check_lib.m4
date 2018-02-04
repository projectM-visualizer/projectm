# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_open62541_check_lib.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_OPEN62541_CHECK_LIB([STATIC-FIRST = `no'], [ACTION-IF-FOUND],
#                          [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Searches for the library file of the open62541 library [1].
#
#   The open62541 library is a cmake-based project, that supplies both a
#   shared and a static library (with different names). The
#   AX_OPEN62541_CHECK_LIB macro can search for both and adds the first one
#   found to the LIBS Makefile variable. The search can be in either order
#   (shared first or static first), which is selectable on macro invocation
#   with the STATIC-FIRST parameter. The user can change this default by
#   invoking configure with either the --with-open62541-shared or
#   --with-open62541-static option (the named one is checked first).
#
#   Either shared or static library can be excluded from checking completely
#   with the options --without-open62541-shared and
#   --without-open62541-static.
#
#   If the with_open62541 shell variable is set to "no" (e.g. from running
#   the AX_OPEN62541_PATH macro and the user giving configure the option
#   "--without-open62541"), then expands ACTION-IF-NOT-FOUND without any
#   checks. The same is true if both --without-open62541-shared and
#   --without-open62541-static are given.
#
#   [1]: <http://open62541.org/>
#
# LICENSE
#
#   Copyright (c) 2016 Olaf Mandel <olaf@mandel.name>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 2

# _AX_OPEN62541_CHECK_LIB_arg(THIS, OTHER)
# ----------------------------------------
# Define the option --with-open62541-THIS, including checking that no
# unknown argument is given or that both --with-open62541-THIS and
# --with-open62541-OTHER are called.
m4_define([_AX_OPEN62541_CHECK_LIB_arg],
[AC_ARG_WITH([open62541-$1],
             [AS_HELP_STRING([--with-open62541-$1],
                             [search for the $1 open62541 library first])],
             [], [])dnl
AS_IF([test x${with_open62541_$1:+set} == xset]dnl
[ -a "x$with_open62541_$1" != xno -a "x$with_open62541_$1" != xyes],
      [AC_MSG_FAILURE([--with-open62541-$1 does not take an argument])],
      [test x${with_open62541_$1:+set} == xset]dnl
[ -a "x$with_open62541_$1" == xyes -a x${with_open62541_$2:+set} == xset]dnl
[ -a "x$with_open62541_$2" == xyes],
      [AC_MSG_FAILURE([--with-open62541-$1 and --with-open62541-$2 are]dnl
[ mutually exclusive])])
])# _AX_OPEN62541_CHECK_LIB_arg

# _AX_OPEN62541_CHECK_LIB_1(SHARED-STATIC, ACTION-IF-FOUND,
#                           ACTION-IF-NOT-FOUND)
# ---------------------------------------------------------
# Checks for the desired library if not forbidden by
# with_open62541_(shared|static) shell variables.
m4_define([_AX_OPEN62541_CHECK_LIB_1],
[m4_pushdef([lib], m4_if([$1], [shared], [open62541],
                         [$1], [static], [open62541-static]))dnl
AS_IF([test "x$with_open62541_$1" != xno],
      [AC_CHECK_LIB(m4_defn([lib]), [UA_Server_new], [$2], [$3])],
      [$3])
m4_popdef([lib])dnl
])# _AX_OPEN62541_CHECK_LIB_1

# AX_OPEN62541_CHECK_LIB([STATIC-FIRST], [ACTION-IF-FOUND],
#                        [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------
AC_DEFUN([AX_OPEN62541_CHECK_LIB],
[m4_pushdef([staticfirst], m4_normalize([$1]))dnl
m4_bmatch(m4_defn([staticfirst]),
          [^\([Nn][Oo]?\|0\)$], [m4_define([staticfirst], [])])dnl
m4_pushdef([first], m4_ifblank(m4_defn([staticfirst]), [shared], [static]))dnl
m4_pushdef([secnd], m4_ifblank(m4_defn([staticfirst]), [static], [shared]))dnl
dnl
_AX_OPEN62541_CHECK_LIB_arg([shared], [static])dnl
_AX_OPEN62541_CHECK_LIB_arg([static], [shared])dnl
AS_IF([test x${with_open62541:+set} == xset -a "x$with_open62541" == xno],
      [$3],
      [test "x$with_open62541_]m4_defn([secnd])[" != xyes],
      [_AX_OPEN62541_CHECK_LIB_1(m4_defn([first]), [$2],
          [_AX_OPEN62541_CHECK_LIB_1(m4_defn([secnd]), [$2], [$3])])],
      [_AX_OPEN62541_CHECK_LIB_1(m4_defn([secnd]), [$2],
          [_AX_OPEN62541_CHECK_LIB_1(m4_defn([first]), [$2], [$3])])])dnl
m4_popdef([staticfirst], [first], [secnd])dnl
])# AX_OPEN62541_CHECK_LIB

# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_fc_check_define.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_FC_CHECK_DEFINE([symbol], [ACTION-IF-FOUND], [ACTION-IF-NOT])
#
# DESCRIPTION
#
#   This macro checks to see if the Fortran pre-processor has a symbol
#   defined.
#
#   Consider a usage like:
#
#     AC_FC_PP_DEFINE()
#     AX_FC_CHECK_DEFINE(__BIGGEST_ALIGNMENT__, [],
#                        AX_APPEND_FLAG([${FC_DEFINE}__BIGGEST_ALIGNMENT__=64],
#                                       [FCFLAGS]))
#
# LICENSE
#
#   Copyright (c) 2016 Timothy Brown <tbrown@freeshell.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_FC_CHECK_DEFINE], [

AS_VAR_PUSHDEF([ax_var],[ax_cv_defined_$1])dnl
AC_LANG_PUSH([Fortran])
ax_fc_check_define_srcext_save=$ac_fc_srcext
AC_FC_PP_SRCEXT([F])
AC_CACHE_CHECK([if $1 is defined], [ax_var],
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[
#ifdef $1
      integer :: ok
#else
      choke me
#endif
    ]])],
    [AS_VAR_SET([ax_var], yes)],
    [AS_VAR_SET([ax_var], no)]))
AS_IF([test AS_VAR_GET([ax_var]) != "no"], [$2], [$3])
ac_fc_srcext=$ax_fc_check_define_srcext_save
AC_LANG_POP()dnl
AS_VAR_POPDEF([ax_var])dnl
])

# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_require_one_func.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_REQUIRE_ONE_FUNC (FUNCTION..., [ACTION-IF-ANY-FOUND], [ACTION-IF-NONE-FOUND])
#
# DESCRIPTION
#
#   AX_REQUIRE_ONE_FUNC is a simple wrapper for AC_CHECK_FUNCS. It calls
#   AC_CHECK_FUNCS on the list of functions named in the first argument,
#   then invokes ACTION-IF-ANY-FOUND if at least one of the functions exists
#   or ACTION-IF-NONE-FOUND if none of the functions exist.
#
#   Here's an example:
#
#     AX_REQUIRE_ONE_FUNC([posix_memalign memalign valloc], ,
#       [AC_MSG_ERROR([unable to allocate page-aligned memory])])
#
# LICENSE
#
#   Copyright (c) 2008 Scott Pakin <pakin@uiuc.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 9

AC_DEFUN([AX_REQUIRE_ONE_FUNC],
[m4_define([ax_1func_cv], [AS_TR_SH(ax_cv_func_any_$1)])
AC_CACHE_VAL([ax_1func_cv],
  [ax_1func_cv=no
   AC_CHECK_FUNCS([$1], [ax_1func_cv="$ax_1func_cv $ac_func"])])
AS_IF([test "$ax_1func_cv" = "no"],
  [$3],
  [ax_1func_cv=`echo $ax_1func_cv | sed 's/^no //'`
   $2])
])

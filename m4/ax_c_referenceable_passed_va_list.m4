# ======================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_c_referenceable_passed_va_list.html
# ======================================================================================
#
# SYNOPSIS
#
#   AX_C_REFERENCEABLE_PASSED_VA_LIST
#
# DESCRIPTION
#
#   Checks whether f(va_list va){ &va; } works as expected.
#
#   This macro uses compile-time detection and so is cross-compile ready.
#
#   C99 mentioned passing a pointer to va_list to other functions (footnote
#   212 of "7.15 Variable arguments <stdarg.h>"). However, f(va_list va) {
#   &va; } produces broken pointer on some environments such as gcc on
#   x86_64, although { va_list va; &va; } works as expected. See the
#   detection code of this file and any of pages
#   https://www.gnu.org/software/autoconf/manual/html_node/Function-Portability.html,
#   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=14557, and
#   http://gcc.gnu.org/bugzilla/show_bug.cgi?id=20951 for further
#   information.
#
#   Although C99 does not define the operations f(va_list va) { &va; } and
#   &va itself as standard (footnotes are declared as "normative part,
#   information only"), certain situations need it. This macro provides a
#   type detection about va_list implementation to deal with the operation.
#
#   Following workaround will probably work on such environments although it
#   does not ensure to be safe and portable. At least it is working on
#   x86_64-unknown-linux-gnu:
#
#    f(va_list va)
#    {
#      va_list *vap;
#
#    #if HAVE_REFERENCEABLE_PASSED_VA_LIST
#        vap = &va;
#    #else
#        vap = (va_list *)va;
#    #endif
#    }
#
# LICENSE
#
#   Copyright (c) 2008 YAMAMOTO Kengo <yamaken@bp.iij4u.or.jp>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 9

AC_DEFUN([AX_C_REFERENCEABLE_PASSED_VA_LIST], [
  AC_CACHE_CHECK([whether f(va_list va){ &va; } works as expected],
    [ax_cv_c_referenceable_passed_va_list],
    [AC_LINK_IFELSE([[
#include <stdarg.h>

volatile va_list g_va;

void
vf(va_list callee_va)
{
  /* typeof(callee_va) differs from typeof(caller_va) by a compiler trick, if
   * va_list is implemented as an array. On such environment, this copy
   * operation fails. */
  g_va = callee_va;
}

void
f(int last, ...)
{
  va_list caller_va;

  va_start(caller_va, last);
  vf(caller_va);  /* passed as &caller_va[0] if va_list is an array type */
  va_end(caller_va);
}

int
main(int argc, char *argv[])
{
  f(0xdeadbeef, 0xfedbeef, 0xfeedee);

  return 0;
}
      ]],
      [ax_cv_c_referenceable_passed_va_list=yes],
      [ax_cv_c_referenceable_passed_va_list=no])])
  if test "x$ax_cv_c_referenceable_passed_va_list" = xyes; then
    AC_DEFINE([HAVE_REFERENCEABLE_PASSED_VA_LIST], [1],
              [Define to 1 if f(va_list va){ &va; } works as expected.])
  fi
])

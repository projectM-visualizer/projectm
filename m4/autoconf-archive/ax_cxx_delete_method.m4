# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_delete_method.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_DELETE_METHOD
#
# DESCRIPTION
#
#   Check whether the C++11 '= delete' syntax, for suppressing undesired
#   implicit methods, is supported.  If it is, the macro DELETE_METHOD is
#   defined to '= delete'; otherwise it is defined to nothing.  Thus, you
#   can write
#
#     class foo {
#       ...
#     private:
#       foo(foo const&) DELETE_METHOD;
#     };
#
#   to delete the 'foo' copy constructor or fall back to the idiom of a
#   private undefined method if the compiler doesn't support this.
#
#   Does not test '= delete' on a template specialization. Does not ensure
#   that the compiler is in C++11 mode.
#
# LICENSE
#
#   Copyright (c) 2012 Zack Weinberg <zackw@panix.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_CXX_DELETE_METHOD], [dnl
  AC_LANG_ASSERT([C++])
  # This compilation should succeed...
  AC_CACHE_CHECK(whether $CXX accepts method deletion,
                 ax_cv_cxx_delete_method_syntax, [
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
      struct foo {
        foo(double);
        foo(int) = delete;
      };
      extern void t(foo const&);
      void tt(double n) { t(n); }
   ]])],
      [ax_cv_cxx_delete_method_syntax=yes],
      [ax_cv_cxx_delete_method_syntax=no])])
  # ... and this one should fail.
  AC_CACHE_CHECK(whether $CXX enforces method deletion,
                 ax_cv_cxx_delete_method_enforced, [
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
      struct foo {
        foo(double);
        foo(int) = delete;
      };
      extern void t(foo const&);
      void tt(int n) { t(n); }
   ]])],
      [ax_cv_cxx_delete_method_enforced=no],
      [ax_cv_cxx_delete_method_enforced=yes])])
  if test $ax_cv_cxx_delete_method_syntax = yes &&
     test $ax_cv_cxx_delete_method_enforced = yes
  then
     AC_DEFINE([DELETE_METHOD], [= delete],
               [Define as `= delete' if your compiler supports C++11 method
                deletion, as nothing otherwise.])
  else
     AC_DEFINE([DELETE_METHOD], [],
               [Define as `= delete' if your compiler supports C++11 method
                deletion, as nothing otherwise.])
  fi
])

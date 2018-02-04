# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_rvalue_references.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_CXX_RVALUE_REFERENCES
#
# DESCRIPTION
#
#   Check whether C++11 rvalue references are supported.  If they are, the
#   macro HAVE_RVALUE_REFERENCES is defined.
#
#   Does not ensure that the compiler is in C++11 mode.
#
# LICENSE
#
#   Copyright (c) 2012 Tudor Bosman <tudorb@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_CXX_RVALUE_REFERENCES], [dnl
  AC_LANG_ASSERT([C++])
  # This compilation should succeed...
  AC_CACHE_CHECK(whether $CXX supports rvalue references,
                 ax_cv_cxx_rvalue_references_supported, [
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
      #include <utility>
      struct foo {
        foo(foo&& other) { }
        foo& operator=(foo&& other) {
          if (&other == this) return *this;
          std::move(other);  // make sure this compiles
          return *this;
        }
      };
    ]])],
      [ax_cv_cxx_rvalue_references_supported=yes],
      [ax_cv_cxx_rvalue_references_supported=no])])
  if test $ax_cv_cxx_rvalue_references_supported = yes
  then
    AC_DEFINE([HAVE_RVALUE_REFERENCES], [],
              [define if the compiler supports rvalue references])
  fi
])

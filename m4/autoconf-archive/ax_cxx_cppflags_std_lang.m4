# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_cppflags_std_lang.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_CXX_CPPFLAGS_STD_LANG(CPP-FLAGS)
#
# DESCRIPTION
#
#   Append to CPP-FLAGS the set of flags that should be passed to the C++
#   preprocessor in order to enable use of C++ features as defined in the
#   ANSI C++ standard (eg. use of standard iostream classes in the `std'
#   namespace, etc.).
#
# LICENSE
#
#   Copyright (c) 2008 Ludovic Courtes <ludo@chbouib.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AU_ALIAS([AC_CXX_CPPFLAGS_STD_LANG], [AX_CXX_CPPFLAGS_STD_LANG])
AC_DEFUN([AX_CXX_CPPFLAGS_STD_LANG],
  [AC_LANG_ASSERT([C++])
   AC_REQUIRE([AX_COMPILER_VENDOR])
   case "$ax_cv_cxx_compiler_vendor" in
     dec|compaq)
      # By default, Compaq CXX has an iostream classes implementation
      # that is _not_ in the `std' namespace.
      $1="$$1 -D__USE_STD_IOSTREAM=1";;
     hp)
      # When `configure' looks for a C++ header (eg. <iostream>) it invokes
      # `aCC -E $CPPFLAGS'.  However, including standard C++ headers like
      # <iostream> fails if `-AA' is not supplied.
      $1="$$1 -AA";;
   esac])

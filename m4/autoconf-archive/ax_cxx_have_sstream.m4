# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_have_sstream.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_SSTREAM
#
# DESCRIPTION
#
#   If the C++ library has a working stringstream, define HAVE_SSTREAM.
#
# LICENSE
#
#   Copyright (c) 2008 Ben Stanley <Ben.Stanley@exemail.com.au>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([AC_CXX_HAVE_SSTREAM], [AX_CXX_HAVE_SSTREAM])
AC_DEFUN([AX_CXX_HAVE_SSTREAM],
[AC_CACHE_CHECK(whether the compiler has stringstream,
ax_cv_cxx_have_sstream,
[AC_REQUIRE([AX_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <sstream>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[stringstream message; message << "Hello"; return 0;],
 ax_cv_cxx_have_sstream=yes, ax_cv_cxx_have_sstream=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_sstream" = yes; then
  AC_DEFINE(HAVE_SSTREAM,,[define if the compiler has stringstream])
fi
])

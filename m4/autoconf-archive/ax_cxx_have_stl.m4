# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_cxx_have_stl.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_STL
#
# DESCRIPTION
#
#   If the compiler supports the Standard Template Library, define HAVE_STL.
#
# LICENSE
#
#   Copyright (c) 2008 Todd Veldhuizen
#   Copyright (c) 2008 Luc Maisonobe <luc@spaceroots.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([AC_CXX_HAVE_STL], [AX_CXX_HAVE_STL])
AC_DEFUN([AX_CXX_HAVE_STL],
[AC_CACHE_CHECK(whether the compiler supports Standard Template Library,
ax_cv_cxx_have_stl,
[AC_REQUIRE([AX_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <list>
#include <deque>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[list<int> x; x.push_back(5);
list<int>::iterator iter = x.begin(); if (iter != x.end()) ++iter; return 0;],
 ax_cv_cxx_have_stl=yes, ax_cv_cxx_have_stl=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_stl" = yes; then
  AC_DEFINE(HAVE_STL,,[define if the compiler supports Standard Template Library])
fi
])

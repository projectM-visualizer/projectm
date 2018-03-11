# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_vector_at.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_VECTOR_AT
#
# DESCRIPTION
#
#   If the implementation of the C++ library provides the method
#   std::vector::at(std::size_t), define HAVE_VECTOR_AT.
#
# LICENSE
#
#   Copyright (c) 2008 Jan Langer <jan@langernetz.de>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([AC_CXX_HAVE_VECTOR_AT], [AX_CXX_HAVE_VECTOR_AT])
AC_DEFUN([AX_CXX_HAVE_VECTOR_AT],
[AC_CACHE_CHECK(whether the compiler has std::vector::at (std::size_t),
ax_cv_cxx_have_vector_at,
[AC_REQUIRE([AX_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <vector>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[vector<int> v (1); v.at (0); return 0;],
 ax_cv_cxx_have_vector_at=yes, ax_cv_cxx_have_vector_at=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_vector_at" = yes; then
 AC_DEFINE(HAVE_VECTOR_AT,,[define if the compiler has the method
std::vector::at (std::size_t)])
fi
])dnl

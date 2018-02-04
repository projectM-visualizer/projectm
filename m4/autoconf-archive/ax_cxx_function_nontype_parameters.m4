# =======================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_function_nontype_parameters.html
# =======================================================================================
#
# SYNOPSIS
#
#   AX_CXX_FUNCTION_NONTYPE_PARAMETERS
#
# DESCRIPTION
#
#   If the compiler supports function templates with non-type parameters,
#   define HAVE_FUNCTION_NONTYPE_PARAMETERS.
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

AU_ALIAS([AC_CXX_FUNCTION_NONTYPE_PARAMETERS], [AX_CXX_FUNCTION_NONTYPE_PARAMETERS])
AC_DEFUN([AX_CXX_FUNCTION_NONTYPE_PARAMETERS],
[AC_CACHE_CHECK(whether the compiler supports function templates with non-type parameters,
ax_cv_cxx_function_nontype_parameters,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
template<class T, int N> class A {};
template<class T, int N> int f(const A<T,N>& x) { return 0; }
],[A<double, 17> z; return f(z);],
 ax_cv_cxx_function_nontype_parameters=yes, ax_cv_cxx_function_nontype_parameters=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_function_nontype_parameters" = yes; then
  AC_DEFINE(HAVE_FUNCTION_NONTYPE_PARAMETERS,,
            [define if the compiler supports function templates with non-type parameters])
fi
])

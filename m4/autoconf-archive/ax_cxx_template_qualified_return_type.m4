# ==========================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_template_qualified_return_type.html
# ==========================================================================================
#
# SYNOPSIS
#
#   AX_CXX_TEMPLATE_QUALIFIED_RETURN_TYPE
#
# DESCRIPTION
#
#   If the compiler supports template-qualified return types, define
#   HAVE_TEMPLATE_QUALIFIED_RETURN_TYPE.
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

AU_ALIAS([AC_CXX_TEMPLATE_QUALIFIED_RETURN_TYPE], [AX_CXX_TEMPLATE_QUALIFIED_RETURN_TYPE])
AC_DEFUN([AX_CXX_TEMPLATE_QUALIFIED_RETURN_TYPE],
[AC_CACHE_CHECK(whether the compiler supports template-qualified return types,
ax_cv_cxx_template_qualified_return_type,
[AC_REQUIRE([AX_CXX_TYPENAME])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
#ifndef HAVE_TYPENAME
 #define typename
#endif
template<class X, class Y> struct promote_trait             { typedef X T; };
template<>                 struct promote_trait<int, float> { typedef float T; };
template<class T> class A { public : A () {} };
template<class X, class Y>
A<typename promote_trait<X,Y>::T> operator+ (const A<X>&, const A<Y>&)
{ return A<typename promote_trait<X,Y>::T>(); }
],[A<int> x; A<float> y; A<float> z = x + y; return 0;],
 ax_cv_cxx_template_qualified_return_type=yes, ax_cv_cxx_template_qualified_return_type=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_template_qualified_return_type" = yes; then
  AC_DEFINE(HAVE_TEMPLATE_QUALIFIED_RETURN_TYPE,,
            [define if the compiler supports template-qualified return types])
fi
])

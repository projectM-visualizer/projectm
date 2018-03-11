# =========================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_template_qualified_base_class.html
# =========================================================================================
#
# SYNOPSIS
#
#   AX_CXX_TEMPLATE_QUALIFIED_BASE_CLASS
#
# DESCRIPTION
#
#   If the compiler supports template-qualified base class specifiers,
#   define HAVE_TEMPLATE_QUALIFIED_BASE_CLASS.
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

AU_ALIAS([AC_CXX_TEMPLATE_QUALIFIED_BASE_CLASS], [AX_CXX_TEMPLATE_QUALIFIED_BASE_CLASS])
AC_DEFUN([AX_CXX_TEMPLATE_QUALIFIED_BASE_CLASS],
[AC_CACHE_CHECK(whether the compiler supports template-qualified base class specifiers,
ax_cv_cxx_template_qualified_base_class,
[AC_REQUIRE([AX_CXX_TYPENAME])
 AC_REQUIRE([AX_CXX_FULL_SPECIALIZATION_SYNTAX])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
#ifndef HAVE_TYPENAME
 #define typename
#endif
class Base1 { public : int f () const { return 1; } };
class Base2 { public : int f () const { return 0; } };
template<class X> struct base_trait        { typedef Base1 base; };
#ifdef HAVE_FULL_SPECIALIZATION_SYNTAX
template<>        struct base_trait<float> { typedef Base2 base; };
#else
                  struct base_trait<float> { typedef Base2 base; };
#endif
template<class T> class Weird : public base_trait<T>::base
{ public :
  typedef typename base_trait<T>::base base;
  int g () const { return base::f (); }
};],[ Weird<float> z; return z.g ();],
 ax_cv_cxx_template_qualified_base_class=yes, ax_cv_cxx_template_qualified_base_class=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_template_qualified_base_class" = yes; then
  AC_DEFINE(HAVE_TEMPLATE_QUALIFIED_BASE_CLASS,,
            [define if the compiler supports template-qualified base class specifiers])
fi
])

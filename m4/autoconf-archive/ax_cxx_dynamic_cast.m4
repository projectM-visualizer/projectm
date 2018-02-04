# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_cxx_dynamic_cast.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_DYNAMIC_CAST
#
# DESCRIPTION
#
#   If the compiler supports dynamic_cast<>, define HAVE_DYNAMIC_CAST.
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

AU_ALIAS([AC_CXX_DYNAMIC_CAST], [AX_CXX_DYNAMIC_CAST])
AC_DEFUN([AX_CXX_DYNAMIC_CAST],
[AC_CACHE_CHECK(whether the compiler supports dynamic_cast<>,
ax_cv_cxx_dynamic_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
class Base { public : Base () {} virtual void f () = 0;};
class Derived : public Base { public : Derived () {} virtual void f () {} };],[
Derived d; Base& b=d; return dynamic_cast<Derived*>(&b) ? 0 : 1;],
 ax_cv_cxx_dynamic_cast=yes, ax_cv_cxx_dynamic_cast=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_dynamic_cast" = yes; then
  AC_DEFINE(HAVE_DYNAMIC_CAST,,[define if the compiler supports dynamic_cast<>])
fi
])

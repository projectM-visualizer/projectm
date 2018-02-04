# ===========================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_templates_as_template_arguments.html
# ===========================================================================================
#
# SYNOPSIS
#
#   AX_CXX_TEMPLATES_AS_TEMPLATE_ARGUMENTS
#
# DESCRIPTION
#
#   If the compiler supports templates as template arguments, define
#   HAVE_TEMPLATES_AS_TEMPLATE_ARGUMENTS.
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

AU_ALIAS([AC_CXX_TEMPLATES_AS_TEMPLATE_ARGUMENTS], [AX_CXX_TEMPLATES_AS_TEMPLATE_ARGUMENTS])
AC_DEFUN([AX_CXX_TEMPLATES_AS_TEMPLATE_ARGUMENTS],
[AC_CACHE_CHECK(whether the compiler supports templates as template arguments,
ax_cv_cxx_templates_as_template_arguments,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
template<class T> class allocator { public : allocator() {}; };
template<class X, template<class Y> class T_alloc>
class A { public : A() {} private : T_alloc<X> alloc_; };
],[A<double, allocator> x; return 0;],
 ax_cv_cxx_templates_as_template_arguments=yes, ax_cv_cxx_templates_as_template_arguments=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_templates_as_template_arguments" = yes; then
  AC_DEFINE(HAVE_TEMPLATES_AS_TEMPLATE_ARGUMENTS,,
            [define if the compiler supports templates as template arguments])
fi
])

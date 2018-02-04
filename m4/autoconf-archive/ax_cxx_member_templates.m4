# ============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_member_templates.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_CXX_MEMBER_TEMPLATES
#
# DESCRIPTION
#
#   If the compiler supports member templates, define HAVE_MEMBER_TEMPLATES.
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

AU_ALIAS([AC_CXX_MEMBER_TEMPLATES], [AX_CXX_MEMBER_TEMPLATES])
AC_DEFUN([AX_CXX_MEMBER_TEMPLATES],
[AC_CACHE_CHECK(whether the compiler supports member templates,
ax_cv_cxx_member_templates,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
template<class T, int N> class A
{ public:
  template<int N2> A<T,N> operator=(const A<T,N2>& z) { return A<T,N>(); }
};],[A<double,4> x; A<double,7> y; x = y; return 0;],
 ax_cv_cxx_member_templates=yes, ax_cv_cxx_member_templates=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_member_templates" = yes; then
  AC_DEFINE(HAVE_MEMBER_TEMPLATES,,[define if the compiler supports member templates])
fi
])

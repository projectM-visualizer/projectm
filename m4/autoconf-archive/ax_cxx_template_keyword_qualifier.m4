# ======================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_template_keyword_qualifier.html
# ======================================================================================
#
# SYNOPSIS
#
#   AX_CXX_TEMPLATE_KEYWORD_QUALIFIER
#
# DESCRIPTION
#
#   If the compiler supports use of the template keyword as a qualifier,
#   define HAVE_TEMPLATE_KEYWORD_QUALIFIER.
#
# LICENSE
#
#   Copyright (c) 2008 Todd Veldhuizen
#   Copyright (c) 2008 Bernardo Innocenti
#   Copyright (c) 2008 Luc Maisonobe <luc@spaceroots.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([AC_CXX_TEMPLATE_KEYWORD_QUALIFIER], [AX_CXX_TEMPLATE_KEYWORD_QUALIFIER])
AC_DEFUN([AX_CXX_TEMPLATE_KEYWORD_QUALIFIER],
[AC_CACHE_CHECK(whether the compiler supports use of the template keyword as a qualifier,
ax_cv_cxx_template_keyword_qualifier,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
  class X
  {
    public:
    template<int> void member() {}
    template<int> static void static_member() {}
  };
  template<class T> void f(T* p)
  {
    p->template member<200>(); // OK: < starts template argument
    T::template static_member<100>(); // OK: < starts explicit qualification
  }
],[X x; f(&x); return 0;],
 ax_cv_cxx_template_keyword_qualifier=yes, ax_cv_cxx_template_keyword_qualifier=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_cxx_template_keyword_qualifier" = yes; then
  AC_DEFINE(HAVE_TEMPLATE_KEYWORD_QUALIFIER,,
            [define if the compiler supports use of the template keyword as a qualifier])
fi
])

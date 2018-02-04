# =======================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_default_template_parameters.html
# =======================================================================================
#
# SYNOPSIS
#
#   AX_CXX_DEFAULT_TEMPLATE_PARAMETERS
#
# DESCRIPTION
#
#   If the compiler supports default template parameters, define
#   HAVE_DEFAULT_TEMPLATE_PARAMETERS.
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

#serial 8

AU_ALIAS([AC_CXX_DEFAULT_TEMPLATE_PARAMETERS], [AX_CXX_DEFAULT_TEMPLATE_PARAMETERS])
AC_DEFUN([AX_CXX_DEFAULT_TEMPLATE_PARAMETERS],
[dnl
  AC_REQUIRE([AX_CXX_TEMPLATES])
  AC_CACHE_CHECK([whether the compiler supports default template parameters],
  [ax_cv_cxx_default_template_parameters],
  [dnl
   AS_IF([test "X$ax_cv_cxx_templates" = "Xyes"],
     [dnl
      AC_LANG_PUSH([C++])
      AC_COMPILE_IFELSE([dnl
        AC_LANG_PROGRAM([
	  template<class T = double, int N = 10> class A {
	    public: int f() {return 0;}
	  };
	  ],
	  [A<float> a; return a.f();])],
	ax_cv_cxx_default_template_parameters=yes,
	ax_cv_cxx_default_template_parameters=no)
	AC_LANG_POP([C++])
      ],
      [ax_cv_cxx_default_template_parameters=no])
   ])
   AS_IF([test "X$ax_cv_cxx_default_template_parameters" = "Xyes"],
         [AC_DEFINE(HAVE_DEFAULT_TEMPLATE_PARAMETERS,,
	   [define if the compiler supports default template parameters])])
])

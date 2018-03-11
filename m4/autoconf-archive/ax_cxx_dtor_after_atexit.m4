# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_dtor_after_atexit.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_CXX_DTOR_AFTER_ATEXIT
#
# DESCRIPTION
#
#   If the C++ compiler calls global destructors after atexit functions,
#   define HAVE_DTOR_AFTER_ATEXIT.
#
#   Conformant behavior is to have global destructors after atexit
#
#   Per Paragraph 3.6.3/1 of the C++11 Standard:
#
#   Destructors (12.4) for initialized objects [..] with static storage
#   duration are called as a result of returning from main and as a result
#   of calling std::exit (18.5).
#
#   And per Paragraph 3.6.3/3:
#
#   If the completion of the initialization of an object with static storage
#   duration is sequenced before a call to std::atexit [..], the call to the
#   function passed to std::atexit is sequenced before the call to the
#   destructor for the object.
#
#   WARNING: If cross-compiling, the test cannot be performed, the default
#   action is to define HAVE_DTOR_AFTER_ATEXIT.
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

#serial 9

AU_ALIAS([AC_CXX_DTOR_AFTER_ATEXIT], [AX_CXX_DTOR_AFTER_ATEXIT])
AC_DEFUN([AX_CXX_DTOR_AFTER_ATEXIT],
[dnl
 AC_CACHE_CHECK(whether the compiler calls global destructors after functions registered through atexit,
   [ax_cv_cxx_dtor_after_atexit],
   [AC_LANG_PUSH([C++])
    AC_RUN_IFELSE([
      AC_LANG_PROGRAM(dnl
	[
          #include <unistd.h>
          #include <cstdlib>

          static int dtor_called = 0;
          class A { public : ~A () { dtor_called = 1; } };
          static A a;

          void f() { _exit(dtor_called); }],
	[
	  atexit (f);
          return 0;
	])],
	ax_cv_cxx_dtor_after_atexit=yes,
	ax_cv_cxx_dtor_after_atexit=no,
	ax_cv_cxx_dtor_after_atexit=yes)
    AC_LANG_POP([C++])
    ])
  AS_IF([test "X$ax_cv_cxx_dtor_after_atexit" = Xyes],
    [AC_DEFINE(HAVE_DTOR_AFTER_ATEXIT,,
            [define if the compiler calls global destructors after functions registered through atexit])])
])

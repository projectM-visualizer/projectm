# =====================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_verbose_terminate_handler.html
# =====================================================================================
#
# SYNOPSIS
#
#   AX_CXX_VERBOSE_TERMINATE_HANDLER
#
# DESCRIPTION
#
#   Check if the terminate handler for the current compiler/standard library
#   prints useful information to stdout/stderr (i.e. the type of the
#   uncaught exception and/or the what() string).
#
#   The intended use case for this check is to help program authors decide
#   if they need to add a top-level try-catch or custom terminate handler to
#   print useful information if an uncaught exception occurs, or if this
#   task can be left to the standard library implementation. Alternatively,
#   it can be used to detect if such output needs to be silenced with a
#   custom terminate handler.
#
#   Output:
#
#   Define HAVE_VERBOSE_TERMINATE_HANDLER if useful information is printed.
#
# LICENSE
#
#   Copyright (c) 2009 Kevin Locke <klocke@digitalenginesoftware.com>
#   Copyright (c) 2008 Lapo Luchini <lapo@lapo.it>
#
#   Copying and distribution of this file, with or without modification, is
#   permitted in any medium, without royalty, provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.  This file is offered as-is, without any warranty.

#serial 8

AC_DEFUN([AX_CXX_VERBOSE_TERMINATE_HANDLER],
[AC_CACHE_CHECK([whether the terminate function prints useful information],
[ax_cv_cxx_verbose_terminate_handler],
[
  AC_LANG_PUSH([C++])
  AC_RUN_IFELSE(
    [AC_LANG_PROGRAM(
      [#include <stdexcept>],
      [throw std::runtime_error("Test of unhandled exception AABBCCDDEE");])],

    dnl The test should never return 0, since it aborts
    [ax_cv_cxx_verbose_terminate_handler=no],

    dnl If the compilation was successful, run it and capture the output
    [AS_IF([test -x conftest$EXEEXT],
      [_output="$(./conftest$EXEEXT 2>&1)"],
      [_output=""])

    dnl Check that the output contains the what() string
    AS_CASE("$_output",
      [*AABBCCDDEE*],

      dnl Check that the output also contains the exception type
      [AS_CASE("$_output",
        [*runtime_error*],
        [ax_cv_cxx_verbose_terminate_handler=yes],
        [ax_cv_cxx_verbose_terminate_handler=no])],
      [ax_cv_cxx_verbose_terminate_handler=no])

    ],

    dnl When cross-compiling, assume messages are not verbose
    [ax_cv_cxx_verbose_terminate_handler=no])
  AC_LANG_POP([C++])
])
if test "$ax_cv_cxx_verbose_terminate_handler" = yes; then
  AC_DEFINE([HAVE_VERBOSE_TERMINATE_HANDLER], 1,
    [define if the terminate function prints useful information])
fi
])

# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_prog_gjs.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_GJS
#
# DESCRIPTION
#
#   AX_PROG_GJS checks for the presence of the JavaScript interpreter GJS
#   (https://wiki.gnome.org/Projects/Gjs) in pkg-config or in the path. If
#   it is not found, an error is issued and configure is halted. If it is
#   found, the path of the interpreter is placed into a variable named GJS,
#   which is declared precious.
#
# LICENSE
#
#   Copyright (c) 2013, 2016 Endless Mobile, Inc.; contributed by Philip Chimento <philip@endlessm.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN_ONCE([AX_PROG_GJS], [
  AC_REQUIRE([PKG_PROG_PKG_CONFIG])
  AC_ARG_VAR([GJS], [Path to GJS interpreter])
  AC_MSG_CHECKING([for GJS])
  AS_IF([pkg-config --exists gjs-1.0], [
    GJS=`pkg-config --variable=gjs_console gjs-1.0`
    AC_MSG_RESULT([(pkg-config) $GJS])
  ], [
    AC_PATH_PROG([GJS], [gjs], [notfound])
    AC_MSG_RESULT([(path) $GJS])
  ])
  AS_IF([test "x$GJS" = "xnotfound"],
    [AC_MSG_ERROR([GJS is required, but was not found. If GJS is
installed, try passing its path in an environment variable,
for example GJS=/path/to/gjs.])])
])

# _AX_GJS_IFELSE(program, [action-if-true], [action-if-false])
# -------------------------------------------------------------
# Comparable to AC_RUN_IFELSE(), but runs the program using GJS
# instead of trying to compile it and link it.
# Used by AX_CHECK_GIRS_GJS and AX_CHECK_GIR_SYMBOLS_GJS.

AC_DEFUN([_AX_GJS_IFELSE], [
  AC_REQUIRE([AX_PROG_GJS])
  echo "$1" >conftest.js
  $GJS conftest.js >>config.log 2>&1
  AS_IF([test $? -eq 0], [$2], [$3])
])

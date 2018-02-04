# =======================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_add_am_trilinos_makefile_export.html
# =======================================================================================
#
# SYNOPSIS
#
#   AX_ADD_AM_TRILINOS_MAKEFILE_EXPORT(EXPORT_SUFFIX [, ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Checks if a file named <Makefile.export.EXPORT_SUFFIX> appears in the
#   $TRILINOS_INCLUDE directory. If so, adds an include for it using the
#   AX_AM_MACROS framework.
#
#   If ACTION-IF-NOT-FOUND is not provided, configure fails.
#
# LICENSE
#
#   Copyright (c) 2009 Rhys Ulerich <rhys.ulerich@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_ADD_AM_TRILINOS_MAKEFILE_EXPORT],[
AC_REQUIRE([AX_TRILINOS_BASE])
AC_REQUIRE([AX_AM_MACROS])

AC_CACHE_CHECK(
    [for file ${TRILINOS_INCLUDE}/Makefile.export.$1],
    [ax_cv_add_am_trilinos_makefile_export_]translit($1,[. ],[_])[_exists],
    [[ax_cv_add_am_trilinos_makefile_export_]translit($1,[. ],[_])[_exists]=no
     test -f "${TRILINOS_INCLUDE}/Makefile.export.$1" && dnl
     [ax_cv_add_am_trilinos_makefile_export_]translit($1,[. ],[_])[_exists]=yes])

if test "${[ax_cv_add_am_trilinos_makefile_export_]translit($1,[. ],[_])[_exists]}" = "yes"
then
AX_ADD_AM_MACRO([
include ${TRILINOS_INCLUDE}/Makefile.export.$1
])
else
    ifelse([$2],,AC_MSG_ERROR([Could not find ${TRILINOS_INCLUDE}/Makefile.export.$1.  Was Trilinos compiled with --enable-export-makefiles?]),[
    AC_MSG_WARN([Could not find ${TRILINOS_INCLUDE}/Makefile.export.$1.  Was Trilinos compiled with --enable-export-makefiles?])
    $2])
fi

])

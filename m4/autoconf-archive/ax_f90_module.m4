# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_f90_module.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_F90_MODULE(MODULE, MODULE-REGEXP, FUNCTION-BODY [, SEARCH-PATH [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
#
# DESCRIPTION
#
#   Set up the compiler flags to use a given fortran 90 module MODULE is the
#   name of the module. MODULE-REGEXP is a regular expression (used by find)
#   matched by the filename of the module. FUNCTION-BODY is the body of a
#   function (including the 'use' statement and the call to a function
#   defined by the module) SEARCH-PATH is a colon-separated list of
#   directories that will be recursively searched for modules files. If
#   empty, the search path will be composed of $prefix, $ac_default_prefix,
#   and all directories exactly one level *above* the directories in
#   $LD_LIBRARY_PATH (the rationale is that when libraries are put in
#   /some/path/lib, the modules are often put in a directory like
#   /some/path/include or /some/path/mod or something similar). An output
#   variable named F90_MODULE_xxx will be set up with the proper flag for
#   substitution in Makefiles (xxx is built from the first argument, with
#   autoconf traditional escapes).
#
# LICENSE
#
#   Copyright (c) 2009 Luc Maisonobe <luc@spaceroots.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AC_DEFUN([AX_F90_MODULE],[
 AC_REQUIRE([AX_F90_MODULE_FLAG])
 AX_F90_INTERNAL_HEADMOD([$1 fortran 90 module],[$2],"$ax_cv_f90_modflag",
                         [$3],AS_TR_SH(F90_MODULE_$1),[$4],[$5],[$6])
])

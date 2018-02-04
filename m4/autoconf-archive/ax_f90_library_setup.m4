# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_f90_library_setup.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_F90_LIBRARY_SETUP(LIBRARY, HEADER-REGEXP, MODULE-REGEXP, LIB-REGEXP, FUNCTION-BODY)
#
# DESCRIPTION
#
#   Convenience macro to set up a fortran 90 library in a simplified way.
#   LIBRARY is the name of the library. HEADER-REGEXP is a regular
#   expression (used by find) matched by the header file to look for (may be
#   empty). MODULE-REGEXP is a regular expression (used by find) matched by
#   the filename of the module (may be empty). LIB-REGEXP is a regular
#   expression (used by find) matched by the filename of the library, this
#   is useful either if the library filename does not follow the traditional
#   libxxx.a or libxxx.so pattern, or if some specific information is
#   embedded into the name, like compiler used, debugging status ...).
#   FUNCTION-BODY is the body of a function (including the 'use' statements
#   and the call to a function defined by the library).
#
#   This macro is a simple wrapper around AX_F90_MODULE and AX_F90_LIBRARY
#   that uses the parameters provided by the end user through a --with-xxx
#   option to set up the search path. Both a module and a library will be
#   tested, the same path will be used for both tests, so the path must be
#   set up with a common parent directory of both the library file and the
#   module file. The macro also automatically updates the FCFLAGS, LDFLAGS
#   and LIBS variables in addition to providing the F90_HEADER_xxx,
#   F90_MODULE_xxx, F90_LDFLAGS_xxx and F90_LIBS_xxx output variables.
#
#   Example: suppose you have /home/nostradamus/esoteric/lib/libalchemy.a
#   and /home/nostradamus/esoteric/mod/alchemy.mod which provides a function
#   transmute_into_gold, you can use the following in you configure.ac:
#
#     AX_F90_MODULE_EXTENSION
#     if test x$ax_cv_f90_modext = xunknown ; then
#       AC_MSG_ERROR([unable to find f90 modules extension])
#     fi
#     AX_F90_LIBRARY_SETUP(alchemy,[],alchemy.$ax_cv_f90_modext,libalchemy*,[
#        use alchemy
#        call transmute_into_gold('lead')
#       ])
#
#   and the user could configure your package using a command like this:
#
#     ./configure --with-alchemy=$HOME/esoteric
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

AC_DEFUN([AX_F90_LIBRARY_SETUP],[
 AC_ARG_WITH([$1],[  --with-$1=path   specify search path form $1 module and library],
   [if test x${withval} = xno ; then
     AC_MSG_WARN([$1 disabled at user option])
    fi],[withval=""])
 if test x$2 != x ; then
   AX_F90_HEADER([$1],[$2],[$5],$withval,[
      FCFLAGS="$FCFLAGS $AS_TR_SH(F90_HEADER_$1)"
     ],[])
 fi
 if test x$3 != x ; then
   AX_F90_MODULE([$1],[$3],[$5],$withval,[
      FCFLAGS="$FCFLAGS $AS_TR_SH(F90_MODULE_$1)"
     ],[])
 fi
 AX_F90_LIBRARY([$1],[$4],[$5],$withval,[
    LDFLAGS="$LDFLAGS $AS_TR_SH(F90_LDFLAGS_$1)"
    LIBS="$AS_TR_SH(F90_LIBS_$1) $LIBS"
   ],[])
])

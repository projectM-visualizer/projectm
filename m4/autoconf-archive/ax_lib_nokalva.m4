# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_lib_nokalva.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_NOKALVA([ACTION-IF-TRUE], [ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   This macro will check for the existence of OSS Nokalva
#   (http://www.oss.com/products/). It does this by checking for the header
#   file asn1.h and the cppsoed library object file. A --with-nokalva option
#   is supported as well. The following output variables are set with
#   AC_SUBST:
#
#     NOKALVA_CPPFLAGS
#     NOKALVA_LDFLAGS
#     NOKALVA_LIBS
#
#   You can use them like this in Makefile.am:
#
#     AM_CPPFLAGS = $(NOKALVA_CPPFLAGS)
#     AM_LDFLAGS = $(NOKALVA_LDFLAGS)
#     program_LDADD = $(NOKALVA_LIBS)
#
#   Additionally, the C preprocessor symbol HAVE_NOKALVA will be defined
#   with AC_DEFINE if Nokalva is available.
#
# LICENSE
#
#   Copyright (c) 2009 Krzysztof Burghardt <krzysztof@burghardt.pl>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 5

AC_DEFUN([AX_LIB_NOKALVA], [
  AH_TEMPLATE([HAVE_NOKALVA], [Define if Nokalva is available])
  AC_ARG_WITH(nokalva, [  --with-nokalva=DIR      prefix for Nokalva library files and headers], [
    if test "$withval" = "no"; then
      ac_nokalva_path=
      $2
    elif test "$withval" = "yes"; then
      ac_nokalva_path=/usr
    else
      ac_nokalva_path="$withval"
    fi
  ],[ac_nokalva_path=/usr])
  if test "$ac_nokalva_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_nokalva_path/include"
    AC_CHECK_HEADER([asn1.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_nokalva_path/lib"
      AC_CHECK_LIB(cppsoed, ossMalloc, [
        AC_SUBST(NOKALVA_CPPFLAGS, [-I$ac_nokalva_path/include])
        AC_SUBST(NOKALVA_LDFLAGS, [-L$ac_nokalva_path/lib])
        AC_SUBST(NOKALVA_LIBS, ["-ldl -lcppsoed -losscpp"])
        AC_DEFINE([HAVE_NOKALVA])
        $1
      ], [
        :
        $2
      ], [-ldl])
      LDFLAGS="$saved_LDFLAGS"
    ], [
      AC_MSG_RESULT([not found])
      $2
    ])
    CPPFLAGS="$saved_CPPFLAGS"
  fi
])

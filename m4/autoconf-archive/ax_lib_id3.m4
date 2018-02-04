# ===========================================================================
#        https://www.gnu.org/software/autoconf-archive/ax_lib_id3.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_ID3([ACTION-IF-TRUE], [ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   This macro will check for the existence of id3lib
#   (http://id3lib.sourceforge.net/). It does this by checking for the
#   header file id3.h and the id3 library object file. A --with-id3lib
#   option is supported as well. The following output variables are set with
#   AC_SUBST:
#
#     ID3_CPPFLAGS
#     ID3_LDFLAGS
#     ID3_LIBS
#
#   You can use them like this in Makefile.am:
#
#     AM_CPPFLAGS = $(ID3_CPPFLAGS)
#     AM_LDFLAGS = $(ID3_LDFLAGS)
#     program_LDADD = $(ID3_LIBS)
#
#   Additionally, the C preprocessor symbol HAVE_ID3LIB will be defined with
#   AC_DEFINE if id3lib is available.
#
# LICENSE
#
#   Copyright (c) 2009 Oskar Liljeblad <oskar@osk.mine.nu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([AC_LIB_ID3], [AX_LIB_ID3])
AC_DEFUN([AX_LIB_ID3], [
  AH_TEMPLATE([HAVE_ID3LIB], [Define if id3lib is available])
  AC_ARG_WITH(id3lib, [  --with-id3lib=DIR       prefix for id3 library files and headers], [
    if test "$withval" = "no"; then
      ac_id3_path=
      $2
    elif test "$withval" = "yes"; then
      ac_id3_path=/usr
    else
      ac_id3_path="$withval"
    fi
  ],[ac_id3_path=/usr])
  if test "$ac_id3_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_id3_path/include"
    AC_CHECK_HEADER([id3.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_id3_path/lib"
      AC_CHECK_LIB(id3, ID3Tag_New, [
        AC_SUBST(ID3_CPPFLAGS, [-I$ac_id3_path/include])
        AC_SUBST(ID3_LDFLAGS, [-L$ac_id3_path/lib])
        AC_SUBST(ID3_LIBS, [-lid3])
	AC_DEFINE([HAVE_ID3LIB])
        $1
      ], [
        :
        $2
      ])
      LDFLAGS="$saved_LDFLAGS"
    ], [
      AC_MSG_RESULT([not found])
      $2
    ])
    CPPFLAGS="$saved_CPPFLAGS"
  fi
])

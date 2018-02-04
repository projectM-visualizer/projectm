# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_lib_upnp.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_UPNP([ACTION-IF-TRUE], [ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   This macro will check for the existence of libupnp
#   (http://upnp.sourceforge.net/). It does this by checking for the header
#   file upnp.h and the upnp library object file. A --with-libupnp option is
#   supported as well. The following output variables are set with AC_SUBST:
#
#     UPNP_CPPFLAGS
#     UPNP_LDFLAGS
#     UPNP_LIBS
#
#   You can use them like this in Makefile.am:
#
#     AM_CPPFLAGS = $(UPNP_CPPFLAGS)
#     AM_LDFLAGS = $(UPNP_LDFLAGS)
#     program_LDADD = $(UPNP_LIBS)
#
#   Additionally, the C preprocessor symbol HAVE_LIBUPNP will be defined
#   with AC_DEFINE if libupnp is available.
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

AU_ALIAS([AC_LIB_UPNP], [AX_LIB_UPNP])
AC_DEFUN([AX_LIB_UPNP], [
  AH_TEMPLATE([HAVE_LIBUPNP], [Define if libupnp is available])
  AC_ARG_WITH(libupnp, [  --with-libupnp=DIR      prefix for upnp library files and headers], [
    if test "$withval" = "no"; then
      ac_upnp_path=
      $2
    elif test "$withval" = "yes"; then
      ac_upnp_path=/usr
    else
      ac_upnp_path="$withval"
    fi
  ],[ac_upnp_path=/usr])
  if test "$ac_upnp_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_upnp_path/include/upnp"
    AC_CHECK_HEADER([upnp.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_upnp_path/lib"
      AC_CHECK_LIB(upnp, UpnpInit, [
        AC_SUBST(UPNP_CPPFLAGS, [-I$ac_upnp_path/include/upnp])
        AC_SUBST(UPNP_LDFLAGS, [-L$ac_upnp_path/lib])
        AC_SUBST(UPNP_LIBS, [-lupnp])
	AC_DEFINE([HAVE_LIBUPNP])
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

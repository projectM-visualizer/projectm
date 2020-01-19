dnl file      : m4/disable-rpath.m4
dnl author    : Boris Kolpackov <boris@codesynthesis.com>
dnl copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
dnl license   : GNU GPL v2; see accompanying LICENSE file
dnl
AC_DEFUN([DISABLE_RPATH],[

AC_MSG_CHECKING([whether to use rpath])
AC_ARG_ENABLE(
  [rpath],
  [AC_HELP_STRING([--disable-rpath], [patch libtool to not use rpath])],
  [libtool_rpath="$enable_rpath"],
  [libtool_rpath="yes"])
AC_MSG_RESULT($libtool_rpath)

# Patch libtool to not use rpath if requested.
#
AC_CONFIG_COMMANDS(
  [libtool-rpath-patch],
  [if test "$libtool_use_rpath" = "no"; then
     sed < libtool > libtool-2 's/^hardcode_libdir_flag_spec.*$'/'hardcode_libdir_flag_spec=" -D__LIBTOOL_NO_RPATH__ "/'
     mv libtool-2 libtool
     chmod 755 libtool
  fi],
  [libtool_use_rpath=$libtool_rpath])
])dnl


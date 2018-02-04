# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_open62541_path.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_OPEN62541_PATH()
#
# DESCRIPTION
#
#   Sets pre-processor and linker search paths for the open62541 library
#   [1].
#
#   The open62541 library is a cmake-based project, that is not always
#   installed to the system: it does not even provide an install target by
#   default. In that case, the location of several directories need to be
#   added to the pre-processor and linker search paths:
#
#     * -I<src>/deps
#     * -I<src>/include
#     * -I<src>/plugins
#     * -I<build>/src_generated
#     * -I<build>
#     * -L<build>
#
#   Here "src" indicates the location of the library source directory and
#   "build" the location of the out-of-source build.
#
#   This macro provides the configure options --with-open62541=dir and
#   --with-open62541-build=dir to set the two directories. This macro will
#   append to CPPFLAGS and LDFLAGS if a dir is supplied. It will also export
#   LD_LIBRARY_PATH with the absolute path of the build dir for the rest of
#   the configure script to use and define a Makefile variable
#   OPEN62541_LDPATH, which contains
#   LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<abs_path_to_build> : this is intended
#   to be used for all commands that need to execute a program using the
#   library (e.g. make check). If paths are set, they are forwarded to
#   distcheck.
#
#   For --with-open62541, the dir is optional: if either the values "no"
#   (from --without-open62541) or "yes" (for no argument) are specified,
#   this macro does nothing but these values may be used by other code. If
#   --with-open62541 is not specified, the variable with_open62541 is not
#   set.
#
#   For --with-open62541-build, the dir is compulsory.
#   --with-open62541-build must not be given without also specifying
#   --with-open62541. If --with-open62541 is given but
#   --with-open62541-build is not, the default value is "<src>/build".
#
#   [1]: <http://open62541.org/>
#
# LICENSE
#
#   Copyright (c) 2016,2017 Olaf Mandel <olaf@mandel.name>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 5

# AX_OPEN62541_PATH()
# -------------------
AC_DEFUN([AX_OPEN62541_PATH],
[AC_ARG_WITH([open62541],
             [AS_HELP_STRING([--with-open62541@<:@=dir@:>@],
                             [set the open62541 library source location])],
             [], [])dnl
AC_ARG_WITH([open62541-build],
            [AS_HELP_STRING([--with-open62541-build=dir],
                            [set the open62541 build dir location]dnl
[ (@<:@default=$src/build@:>@)])],
            [], [])dnl
[with_open62541=${with_open62541%/}]
AS_IF([test x${with_open62541_build:+set} != xset],
      [AS_IF([test x${with_open62541:+set} == xset]dnl
[ -a "x$with_open62541" != xno -a "x$with_open62541" != xyes],
             [with_open62541_build=$with_open62541/build])],
      [test "x$with_open62541_build" == xno]dnl
[ -o "x$with_open62541_build" == xyes],
      [AC_MSG_FAILURE([--with-open62541-build needs its argument])],
      [test x${with_open62541:+set} != xset -o "x$with_open62541" == xyes],
      [AC_MSG_FAILURE([--with-open62541-build also requires]dnl
[ --with-open62541 to set a directory])],
      [with_open62541_build=${with_open62541_build%/}])
dnl
AS_IF([test x${with_open62541:+set} == xset -a "x$with_open62541" != xno]dnl
[ -a "x$with_open62541" != xyes],
      [CPPFLAGS="$CPPFLAGS${CPPFLAGS:+ }-I$with_open62541/deps]dnl
[ -I$with_open62541/include -I$with_open62541/plugins]dnl
[ -I$with_open62541_build/src_generated -I$with_open62541_build"
LDFLAGS="$LDFLAGS${LDFLAGS:+ }-L$with_open62541_build"
with_open62541_abs=`(
    cd "$with_open62541"
    pwd)`
with_open62541_build_abs=`(
    cd "$with_open62541_build"
    pwd)`
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH${LD_LIBRARY_PATH:+:}]dnl
[$with_open62541_build_abs"
AC_SUBST([OPEN62541_LDPATH], ["LD_LIBRARY_PATH=\$\$LD_LIBRARY_PATH]dnl
[\$\${LD_LIBRARY_PATH:+:}$with_open62541_build_abs"])
AC_SUBST([AM_DISTCHECK_CONFIGURE_FLAGS], ["$AM_DISTCHECK_CONFIGURE_FLAGS]dnl
[${AM_DISTCHECK_CONFIGURE_FLAGS:+ }--with-open62541=$with_open62541_abs"])
AS_IF([test x$with_open62541/build != xwith_open62541_build],
      [AC_SUBST([AM_DISTCHECK_CONFIGURE_FLAGS],
                ["$AM_DISTCHECK_CONFIGURE_FLAGS]dnl
[ --with-open62541-build=$with_open62541_build_abs"])])
])
])# AX_OPEN62541_PATH

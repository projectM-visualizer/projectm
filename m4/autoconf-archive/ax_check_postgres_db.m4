# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_check_postgres_db.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_POSTGRES_DB([ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro tries to find the headers and libraries for the PostgreSQL
#   database to build client applications.
#
#   If includes are found, the variable PQINCPATH will be set, and
#   have_pqinc will be 'yes'. If libraries are found, the variable PQLIBPATH
#   will be set, and have_pqlib will be 'yes'. If both PQINCPATH and
#   PQLIBPATH are found, HAVE_LIBPQ will be set to 'yes' (and AC_DEFINEd),
#   and -lpq will be added to the beginning of LIBS. If their respective
#   conditions are not met, have_pqinc, have_pqlib, HAVE_LIBPQ, will be
#   'no'.
#
#   This macro does fails if either the headers or the library are not
#   found, unless ACTION-IF-NOT-FOUND is not empty. When both are found,
#   ACTION-IF-FOUND, if defined, is executed after the variables have been
#   defined. If --with-pgsql=no (or equivalents) is given, nothing happens.
#
#   An automake conditional can be subsequently defined as
#
#     AM_CONDITIONAL([HAVE_LIBPQ], [test x$HAVE_LIBPQ = x$yes])
#
#   in configure.ac.
#
# LICENSE
#
#   Copyright (c) 2008 Christian Toepp <c.toepp@gmail.com>
#   Copyright (c) 2012 Olivier Mehani <shtrom-ax@ssji.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 12

AU_ALIAS([CT_CHECK_POSTGRES_DB], [AX_CHECK_POSTGRES_DB])
AC_DEFUN([AX_CHECK_POSTGRES_DB], [

pushdef([ACTION_IF_FOUND],$1)
pushdef([ACTION_IF_NOT_FOUND],$2)

AC_ARG_WITH(pgsql,
	[AS_HELP_STRING([--with-pgsql[[=PREFIX]]],
			[Prefix of your PostgreSQL installation @<:@PREFIX@:>@])],
	[pg_prefix=$withval], [pg_prefix=])
AC_ARG_WITH(pgsql-inc,
	[AS_HELP_STRING([--with-pgsql-inc=[[PATH]]],
			[Path to the include directory of PostgreSQL @<:@INCLUDEDIR@:>@])],
	[pg_inc=$withval], [pg_inc=])
AC_ARG_WITH(pgsql-lib,
	[AS_HELP_STRING([--with-pgsql-lib=[[PATH]]],
			[Path to the libraries of PostgreSQL @<:@LIBDIR@:>@])],
	[pg_lib=$withval], [pg_lib=])

have_pqinc=no
have_pqlib=no
HAVE_LIBPQ=no

AS_IF([test "$pg_prefix" != "no"],[
       AS_IF([test "$pg_prefix" != "yes" && test "$pg_prefix" != ""], [
	      PQINCPATH="-I$pg_prefix/include $PQINCPATH"
	      PQLIBPATH="-L$pg_prefix/lib $PQLIBPATH"
	      ])
       AS_IF([test "$pg_inc" != ""], [PQINCPATH="-I$pg_inc $PQINCPATH"])
       AS_IF([test "$pg_lib" != ""], [PQLIBPATH="-L$pg_lib $PQLIBPATH"])

       oldCPPFLAGS=$CPPFLAGS
       oldLDFLAGS=$LDFLAGS

       CPPFLAGS="$PQINCPATH $CPPFLAGS"
       LDFLAGS="$PQLIBPATH $LDFLAGS"

       AC_CHECK_HEADER([libpq-fe.h], [have_pqinc=yes])
       AC_CHECK_LIB([pq], [PQconnectdb], [
		     dnl We only search for libpq, so we know why we're here
		     LIBPQ_LIBS=-lpq
		     have_pqlib=yes
		     ])

       HAVE_LIBPQ=`(test x$have_pqinc = xyes && test x$have_pqlib = xyes && echo yes) || echo no`

       CPPFLAGS=$oldCPPFLAGS
       LDFLAGS=$oldLDFLAGS

       AC_SUBST(PQINCPATH)
       AC_SUBST(PQLIBPATH)
       AC_SUBST(LIBPQ_LIBS)

       AS_IF([test x$HAVE_LIBPQ = xyes],[
	      AC_DEFINE([HAVE_LIBPQ], [1], [Define if libpq is installed])
	      ACTION_IF_FOUND
	      ],
	      [m4_ifset([ACTION_IF_NOT_FOUND],[ACTION_IF_NOT_FOUND],
			  [AC_MSG_ERROR(some support files for PostgreSQL were missing)])])
])

popdef([ACTION_IF_FOUND])
popdef([ACTION_IF_NOT_FOUND])

])

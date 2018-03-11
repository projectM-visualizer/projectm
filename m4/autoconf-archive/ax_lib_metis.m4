# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_lib_metis.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_METIS([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro searches for the METIS library in the user specified
#   location. The user may specify the location either by defining the
#   environment variable METIS or by using the --with-metis option to
#   configure. If the environment variable is defined it has precedent over
#   everything else. If no location was specified then it searches in
#   /usr/lib and /usr/local/lib for the library and in /usr/include and
#   /usr/local/include for the header files. Upon successful completion the
#   variables METIS_LIB and METIS_INCLUDE are set.
#
#   ACTION-IF-FOUND is a list of shell commands to run if a METIS library is
#   found, and ACTION-IF-NOT-FOUND is a list of commands to run it if it is
#   not found. If ACTION-IF-FOUND is not specified, the default action will
#   define HAVE_METIS. If ACTION-IF-NOT-FOUND is not specified then an error
#   will be generated halting configure.
#
# LICENSE
#
#   Copyright (c) 2008 Ben Bergen <ben@cs.fau.de>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AU_ALIAS([IMMDX_LIB_METIS], [AX_LIB_METIS])
AC_DEFUN([AX_LIB_METIS], [
	AC_MSG_CHECKING(for METIS library)
	AC_REQUIRE([AC_PROG_CC])
	#
	# User hints...
	#
	AC_ARG_VAR([METIS], [METIS library location])
	AC_ARG_WITH([metis],
		[AS_HELP_STRING([--with-metis],
		[user defined path to METIS library])],
		[
			if test -n "$METIS" ; then
				AC_MSG_RESULT(yes)
				with_metis=$METIS
			elif test "$withval" != no ; then
				AC_MSG_RESULT(yes)
				with_metis=$withval
			else
				AC_MSG_RESULT(no)
			fi
		],
		[
			if test -n "$METIS" ; then
				with_metis=$METIS
				AC_MSG_RESULT(yes)
			else
				with_metis=/usr
				if test ! -f "$with_metis/include/metis.h" ; then
					with_metis=/usr/local
					if test ! -f "$with_metis/include/metis.h" ; then
						with_metis=""
						AC_MSG_RESULT(failed)
					else
						AC_MSG_RESULT(yes)
					fi
				else
					AC_MSG_RESULT(yes)
				fi
			fi
		])
	#
	# locate METIS library
	#
		if test -n "$with_metis" ; then
			old_CFLAGS=$CFLAGS
			old_LDFLAGS=$LDFLAGS
			CFLAGS="-I$with_metis/include"
			LDFLAGS="-L$with_metis/lib"

			AC_LANG_SAVE
			AC_LANG_C

			AC_CHECK_LIB(metis, METIS_PartMeshDual,
				[metis_lib=yes], [metis_lib=yes], [-lm])
			AC_CHECK_HEADER(metis.h, [metis_h=yes],
				[metis_h=no], [/* check */])

			AC_LANG_RESTORE

			CFLAGS=$old_CFLAGS
			LDFLAGS=$old_LDFLAGS

			AC_MSG_CHECKING(METIS in $with_metis)
			if test "$metis_lib" = "yes" -a "$metis_h" = "yes" ; then
				AC_SUBST(METIS_INCLUDE, [-I$with_metis/include])
				AC_SUBST(METIS_LIB, [-L$with_metis/lib])
				AC_MSG_RESULT(ok)
			else
				AC_MSG_RESULT(failed)
			fi
		fi
		#
		#
		#
		if test x = x"$METIS_LIB" ; then
			ifelse([$2],,[AC_MSG_ERROR(Failed to find valid METIS library)],[$2])
			:
		else
			ifelse([$1],,[AC_DEFINE(HAVE_METIS,1,[Define if you have METIS library])],[$1])
			:
		fi
	])dnl AX_LIB_METIS

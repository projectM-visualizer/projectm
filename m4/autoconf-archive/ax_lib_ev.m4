# ===========================================================================
#        https://www.gnu.org/software/autoconf-archive/ax_lib_ev.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_EV([VERSION],[ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Checks for libev mimicking pkg-config's way of doing things for as long
#   as upstream doesn't provide a .pc file. If successful and provided
#   expand ACTION-IF-FOUND, otherwise expand ACTION-IF-NOT-FOUND, or, if
#   omitted, error out like pkg-config does.
#
#   Defines libev_LIBS and libev_CFLAGS.
#
# LICENSE
#
#   Copyright (c) 2012 Sebastian Freundt <freundt@fresse.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_CHECK_EV_H], [
dnl AX_CHECK_EV_H([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl   defs: ax_cv_header_ev_h yes|no
## assume libev_CFLAGS have been set
	saved_CPPFLAGS="${CPPFLAGS}"
	CPPFLAGS="${CPPFLAGS} ${libev_CFLAGS}"
	## not so fast, we need ev.h checks
	AC_CHECK_HEADER([ev.h], [
		ax_cv_header_ev_h="yes"
		$1
	], [$2])
	CPPFLAGS="${saved_CPPFLAGS}"
])

AU_ALIAS([AC_CHECK_LIBEV], [AX_LIB_EV])
AC_DEFUN([AX_LIB_EV], [
	pushdef([VERSION], [m4_default([$1], [>= 4.0])])
	pushdef([ACTION_IF_FOUND], [$2])
	pushdef([ACTION_IF_NOT_FOUND], [$3])

	AC_CACHE_VAL([ax_cv_feat_libev], [
		## assume failure
		ax_cv_feat_libev="no"

		PKG_CHECK_MODULES([libev], [libev ]VERSION[], [:], [:])

		## CHECK_MODULES' if_found case
		AX_CHECK_EV_H([
			ax_cv_feat_libev="yes"

			if test -z "${pkg_cv_libev_LIBS}"; then
				libev_LIBS="${libev_LIBS} -lev"
			fi

			ACTION_IF_FOUND
		], [
			m4_default([]ACTION_IF_NOT_FOUND[], [AC_MSG_ERROR([dnl
Package requirements (libev version) were not met.

Consider adjusting the PKG_CONFIG_PATH environment variable if you
installed software in a non-standard prefix.

Alternatively, you may set the environment variables libev_CFLAGS
and libev_LIBS to avoid the need to call pkg-config.
See the pkg-config man page for more details.])
			])
		])
	])

	popdef([ACTION_IF_NOT_FOUND])
	popdef([ACTION_IF_FOUND])
	popdef([VERSION])
])dnl AX_LIB_EV

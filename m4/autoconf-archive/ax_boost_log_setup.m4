# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_boost_log_setup.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_LOG_SETUP
#
# DESCRIPTION
#
#   Test for LogSetup library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_LOG_SETUP_LIB)
#
#   And sets:
#
#     HAVE_BOOST_LOG_SETUP
#
# LICENSE
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#   Copyright (c) 2013 Daniel Casimiro <dan.casimiro@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_BOOST_LOG_SETUP],
[
	AC_ARG_WITH([boost-log_setup],
	AS_HELP_STRING([--with-boost-log_setup@<:@=special-lib@:>@],
		[use the Log_setup library from boost - it is possible to specify a certain library for the linker
			e.g. --with-boost-log_setup=boost_log_setup-gcc-mt ]),
        [
        if test "$withval" = "no"; then
	    want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_log_setup_lib=""
        else
	    want_boost="yes"
	    ax_boost_user_log_setup_lib="$withval"
	fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
		AC_REQUIRE([AC_PROG_CC])
		AC_REQUIRE([AC_CANONICAL_BUILD])
	        AC_REQUIRE([AX_BOOST_LOG])

		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

		AC_SUBST(BOOST_CPPFLAGS)

		AC_DEFINE(HAVE_BOOST_LOG_SETUP,,
			[define if the Boost::Log_setup library is available])

		BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

		if test "x$ax_boost_user_log_setup_lib" = "x"; then
			for libextension in `ls $BOOSTLIBDIR/libboost_log_setup*.so* $BOOSTLIBDIR/libboost_log_setup*.dylib* $BOOSTLIBDIR/libboost_log_setup*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_log_setup.*\)\.so.*$;\1;' -e 's;^lib\(boost_log_setup.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_log_setup.*\)\.a.*$;\1;'` ; do
				ax_lib=${libextension}
				AC_CHECK_LIB($ax_lib, exit,
					[BOOST_LOG_SETUP_LIB="-l$ax_lib"; AC_SUBST(BOOST_LOG_SETUP_LIB) link_log_setup="yes"; break],
					[link_log_setup="no"])
			done

			if test "x$link_log_setup" != "xyes"; then
				for libextension in `ls $BOOSTLIBDIR/boost_log_setup*.dll* $BOOSTLIBDIR/boost_log_setup*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_log_setup.*\)\.dll.*$;\1;' -e 's;^\(boost_log_setup.*\)\.a.*$;\1;'` ; do
					ax_lib=${libextension}
					AC_CHECK_LIB($ax_lib, exit,
						[BOOST_LOG_SETUP_LIB="-l$ax_lib"; AC_SUBST(BOOST_LOG_SETUP_LIB) link_log_setup="yes"; break],
						[link_log_setup="no"])
				done
			fi

		else
			for ax_lib in $ax_boost_user_log_setup_lib boost_log_setup-$ax_boost_user_log_setup_lib; do
				AC_CHECK_LIB($ax_lib, exit,
					[BOOST_LOG_SETUP_LIB="-l$ax_lib"; AC_SUBST(BOOST_LOG_SETUP_LIB) link_log_setup="yes"; break],
					[link_log_setup="no"])
			done

		fi

		if test "x$ax_lib" = "x"; then
			AC_MSG_ERROR(Could not find a version of the library!)
		fi

		if test "x$link_log_setup" = "xno"; then
			AC_MSG_ERROR(Could not link against $ax_lib !)
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
		LDFLAGS="$LDFLAGS_SAVED"
	fi
])

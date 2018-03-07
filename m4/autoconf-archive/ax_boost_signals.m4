# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_boost_signals.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_SIGNALS
#
# DESCRIPTION
#
#   Test for Signals library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_SIGNALS_LIB)
#
#   And sets:
#
#     HAVE_BOOST_SIGNALS
#
# LICENSE
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 22

AC_DEFUN([AX_BOOST_SIGNALS],
[
	AC_ARG_WITH([boost-signals],
	AS_HELP_STRING([--with-boost-signals@<:@=special-lib@:>@],
                   [use the Signals library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-signals=boost_signals-gcc-mt-d ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_signals_lib=""
        else
		    want_boost="yes"
		ax_boost_user_signals_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Signals library is available,
					   ax_cv_boost_signals,
        [AC_LANG_PUSH([C++])
		 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/signal.hpp>
											]],
                                  [[boost::signal<void ()> sig;
                                    return 0;
                                  ]])],
                           ax_cv_boost_signals=yes, ax_cv_boost_signals=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_signals" = "xyes"; then
			AC_DEFINE(HAVE_BOOST_SIGNALS,,[define if the Boost::Signals library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
            if test "x$ax_boost_user_signals_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_signals*.so* $BOOSTLIBDIR/libboost_signals*.dylib* $BOOSTLIBDIR/libboost_signals*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_signals.*\)\.so.*$;\1;' -e 's;^lib\(boost_signals.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_signals.*\)\.a.*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_SIGNALS_LIB="-l$ax_lib"; AC_SUBST(BOOST_SIGNALS_LIB) link_signals="yes"; break],
                                 [link_signals="no"])
				done
                if test "x$link_signals" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_signals*.dll* $BOOSTLIBDIR/boost_signals*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_signals.*\)\.dll.*$;\1;' -e 's;^\(boost_signals.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_SIGNALS_LIB="-l$ax_lib"; AC_SUBST(BOOST_SIGNALS_LIB) link_signals="yes"; break],
                                 [link_signals="no"])
				done
                fi

            else
               for ax_lib in $ax_boost_user_signals_lib boost_signals-$ax_boost_user_signals_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [BOOST_SIGNALS_LIB="-l$ax_lib"; AC_SUBST(BOOST_SIGNALS_LIB) link_signals="yes"; break],
                                   [link_signals="no"])
                  done

            fi
            if test "x$ax_lib" = "x"; then
                AC_MSG_ERROR(Could not find a version of the library!)
            fi
			if test "x$link_signals" != "xyes"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

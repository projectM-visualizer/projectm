# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_boost_wave.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_WAVE
#
# DESCRIPTION
#
#   Test for Wave library from the Boost C++ libraries. The macro requires a
#   preceding call to AX_BOOST_BASE. Further documentation is available at
#   <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_WAVE_LIB)
#
#   And sets:
#
#     HAVE_BOOST_WAVE
#
# LICENSE
#
#   Copyright (c) 2009 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2009 Michael Tindal
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 16

AC_DEFUN([AX_BOOST_WAVE],
[
	AC_ARG_WITH([boost-wave],
	AS_HELP_STRING([--with-boost-wave@<:@=special-lib@:>@],
                   [use the Wave library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-wave=boost_wave-gcc-mt-d-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_wave_lib=""
        else
		    want_boost="yes"
		ax_boost_user_wave_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)
    AM_CONDITIONAL(ROSE_USE_BOOST_WAVE, test "x$want_boost" = "xyes"  )

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

		LIBS_SAVED=$LIBS
		LIBS="$LIBS $BOOST_THREAD_LIB"
		export LIBS

        AC_CACHE_CHECK(whether the Boost::Wave library is available,
					   ax_cv_boost_wave,
        [AC_LANG_PUSH([C++])
			 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/wave.hpp>
												]],
                                   [[boost::wave::token_category r; return 0;]])],
                   ax_cv_boost_wave=yes, ax_cv_boost_wave=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_wave" = "xyes"; then

			AC_DEFINE(HAVE_BOOST_WAVE,,[define if the Boost::Wave library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
            if test "x$ax_boost_user_wave_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_wave*.so* $BOOSTLIBDIR/libboost_wave*.dylib* $BOOSTLIBDIR/libboost_wave*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_wave.*\)\.so.*$;\1;' -e 's;^lib\(boost_wave.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_wave.*\)\.a.*$;\1;' ` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_WAVE_LIB="-l$ax_lib"; AC_SUBST(BOOST_WAVE_LIB) link_wave="yes"; break],
                                 [link_wave="no"])
				done
                if test "x$link_wave" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_wave*.dll* $BOOSTLIBDIR/boost_wave*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_wave.*\)\.dll.*$;\1;' -e 's;^\(boost_wave.*\)\.a.*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_WAVE_LIB="-l$ax_lib"; AC_SUBST(BOOST_WAVE_LIB) link_wave="yes"; break],
                                 [link_wave="no"])
				done
                fi

            else
               for ax_lib in $ax_boost_user_wave_lib boost_wave-$ax_boost_user_wave_lib; do
				      AC_CHECK_LIB($ax_lib, exit,
                                   [BOOST_WAVE_LIB="-l$ax_lib"; AC_SUBST(BOOST_WAVE_LIB) link_wave="yes"; break],
                                   [link_wave="no"])
               done
            fi
            if test "x$ax_lib" = "x"; then
                AC_MSG_ERROR(Could not find a version of the library!)
            fi
			if test "x$link_wave" != "xyes"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
            # DQ (4/7/2006): build a macro to use in source code to know when WAVE is to be used.
            AC_DEFINE([USE_ROSE_BOOST_WAVE_SUPPORT],1,[Controls use of BOOST WAVE support in ROSE.])

		else
            AC_DEFINE([USE_ROSE_BOOST_WAVE_SUPPORT],0,[Controls use of BOOST WAVE support in ROSE.])

		fi



		CPPFLAGS="$CPPFLAGS_SAVED"
		LDFLAGS="$LDFLAGS_SAVED"
		LIBS="$LIBS_SAVED"

	fi
])

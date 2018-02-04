# ===============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_boost_test_exec_monitor.html
# ===============================================================================
#
# SYNOPSIS
#
#   AX_BOOST_TEST_EXEC_MONITOR
#
# DESCRIPTION
#
#   Test for Test_Exec_Monitor library from the Boost C++ libraries. The
#   macro requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_TEST_EXEC_MONITOR_LIB)
#
#   And sets:
#
#     HAVE_BOOST_TEST_EXEC_MONITOR
#
# LICENSE
#
#   Copyright (c) 2008 Dodji Seketeli <dodji@seketeli.org>
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 21

AC_DEFUN([AX_BOOST_TEST_EXEC_MONITOR],
[
	AC_ARG_WITH([boost-test-exec-monitor],
	AS_HELP_STRING([--with-boost-test-exec-monitor@<:@=special-lib@:>@],
                   [use the Test_Exec_Monitor library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-test-exec-monitor=boost_test_exec_monitor-gcc ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_test_exec_monitor_lib=""
        else
		    want_boost="yes"
		ax_boost_user_test_exec_monitor_lib="$withval"
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

        AC_CACHE_CHECK(whether the Boost::Test_Exec_Monitor library is available,
					   ax_cv_boost_test_exec_monitor,
        [AC_LANG_PUSH([C++])
			 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/test/test_tools.hpp>]],
                                    [[int i=1 ; BOOST_REQUIRE(i==1); ; return 0;]])],
                   ax_cv_boost_test_exec_monitor=yes, ax_cv_boost_test_exec_monitor=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_test_exec_monitor" = "xyes"; then
			AC_DEFINE(HAVE_BOOST_TEST_EXEC_MONITOR,,[define if the Boost::Test_Exec_Monitor library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

            if test "x$ax_boost_user_test_exec_monitor_lib" = "x"; then
			saved_ldflags="${LDFLAGS}"

                for monitor_library in `ls $BOOSTLIBDIR/libboost_test_exec_monitor*.so* $BOOSTLIBDIR/libboost_test_exec_monitor*.dylib* $BOOSTLIBDIR/libboost_test_exec_monitor*.a* 2>/dev/null` ; do
                    if test -r $monitor_library ; then
                       libextension=`echo $monitor_library | sed 's,.*/,,' | sed -e 's;^lib\(boost_test_exec_monitor.*\)\.so.*$;\1;' -e 's;^lib\(boost_test_exec_monitor.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_test_exec_monitor.*\)\.a.*$;\1;'`
                       ax_lib=${libextension}
                       link_test_exec_monitor="yes"
                    else
                       link_test_exec_monitor="no"
                    fi

			      if test "x$link_test_exec_monitor" = "xyes"; then
                      BOOST_TEST_EXEC_MONITOR_LIB="-l$ax_lib"
                      AC_SUBST(BOOST_TEST_EXEC_MONITOR_LIB)
					  break
				  fi
                done
                if test "x$link_test_exec_monitor" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_test_exec_monitor*.dll* $BOOSTLIBDIR/boost_test_exec_monitor*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_test_exec_monitor.*\)\.dll.*$;\1;' -e 's;^\(boost_test_exec_monitor.*\)\.a.*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_TEST_EXEC_MONITOR_LIB="-l$ax_lib"; AC_SUBST(BOOST_TEST_EXEC_MONITOR_LIB) link_test_exec_monitor="yes"; break],
                                 [link_test_exec_monitor="no"])
				done
                fi

            else
                link_test_exec_monitor="no"
			saved_ldflags="${LDFLAGS}"
                for ax_lib in boost_test_exec_monitor-$ax_boost_user_test_exec_monitor_lib $ax_boost_user_test_exec_monitor_lib ; do
                   if test "x$link_test_exec_monitor" = "xyes"; then
                      break;
                   fi
                   for monitor_library in `ls $BOOSTLIBDIR/lib${ax_lib}.so* $BOOSTLIBDIR/lib${ax_lib}.a* 2>/dev/null` ; do
                   if test -r $monitor_library ; then
                       libextension=`echo $monitor_library | sed 's,.*/,,' | sed -e 's;^lib\(boost_test_exec_monitor.*\)\.so.*$;\1;' -e 's;^lib\(boost_test_exec_monitor.*\)\.a*$;\1;'`
                       ax_lib=${libextension}
                       link_test_exec_monitor="yes"
                    else
                       link_test_exec_monitor="no"
                    fi

				if test "x$link_test_exec_monitor" = "xyes"; then
                        BOOST_TEST_EXEC_MONITOR_LIB="-l$ax_lib"
                        AC_SUBST(BOOST_TEST_EXEC_MONITOR_LIB)
					    break
				    fi
                  done
               done
            fi
            if test "x$ax_lib" = "x"; then
                AC_MSG_ERROR(Could not find a version of the library!)
            fi
			if test "x$link_test_exec_monitor" != "xyes"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_lib_cgal_core.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_CGAL_CORE([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Test for the CGAL_Core library.
#
#   By using the "--with-cgal=" option, define a special installation
#   directory. If CGAL is not found there, the script will fail immediately.
#   Otherwise, $CGAL_HOME is searched, then standard system locations.
#
#   NOTE: This script depends on BOOST_CPPFLAGS, so be sure to run
#   AX_BOOST_BASE in advance.
#
#   This macro calls:
#
#     AC_SUBST(CGAL_CPPFLAGS)
#     AC_SUBST(CGAL_LDFLAGS)
#
#   And sets:
#
#     HAVE_CGAL
#
# LICENSE
#
#   Copyright (c) 2010 Sebastian Hegler <sebastian.hegler@tu-dresden.de>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 4

AC_DEFUN([AX_LIB_CGAL_CORE],[

dnl guess from env, or use given value
AC_ARG_WITH([cgal],
	AS_HELP_STRING([--with-cgal@<:@=DIR@:>@],
			[location of cgal installation, default $CGAL_HOME]),
	[ac_cgal_dirs="$withval"],
	[ac_cgal_dirs="$CGAL_HOME"' /usr /usr/local /opt /opt/local']
)

AC_LANG_PUSH([C++])

for ac_cgal_iterate in $ac_cgal_dirs ; do
	CPPFLAGS_SAVED="$CPPFLAGS"
	CGAL_CPPFLAGS="-I$ac_cgal_iterate/include"
	CPPFLAGS="$CPPFLAGS $CGAL_CPPFLAGS $BOOST_CPPFLAGS"
	export CPPFLAGS

	CGAL_LDFLAGS="-L$ac_cgal_iterate/lib -lCGAL -lCGAL_Core"
	LDFLAGS_SAVED="$LDFLAGS"
	LDFLAGS="$LDFLAGS $CGAL_LDFLAGS"
	export LDFLAGS

	AC_MSG_CHECKING([whether CGAL is available in $ac_cgal_iterate])
	dnl This test program is taken from:
	dnl http://www.cgal.org/Manual/latest/examples/Convex_hull_2/vector_convex_hull_2.cpp
	AC_RUN_IFELSE(
		[AC_LANG_PROGRAM(
		[
		[@%:@include <vector>]
		[@%:@include <CGAL/Exact_predicates_inexact_constructions_kernel.h>]
		[@%:@include <CGAL/convex_hull_2.h>]
		[typedef CGAL::Exact_predicates_inexact_constructions_kernel K;]
		[typedef K::Point_2 Point_2;]
		[typedef std::vector<Point_2> Points;]
		],
		[
		[Points points, result;
		points.push_back(Point_2(0,0));
		points.push_back(Point_2(10,0));
		points.push_back(Point_2(10,10));
		points.push_back(Point_2(6,5));
		points.push_back(Point_2(4,1));
		CGAL::convex_hull_2(points.begin(),points.end(),std::back_inserter(result));
		//std::cout << result.size() << " points on the convex hull" << std::endl;]
		])]
		,[ac_cgal=yes],[ac_cgal=no])

	LDFLAGS="$LDFLAGS_SAVED"
	export LDFLAGS
	CPPFLAGS="$CPPFLAGS_SAVED"
	export CPPFLAGS

	if test $ac_cgal = yes ; then
		AC_MSG_RESULT([yes])
		break
	else
		AC_MSG_RESULT([no])
	fi
done

AC_LANG_POP([C++])

if test $ac_cgal = yes ; then
	AC_DEFINE(HAVE_CGAL,[1],[Indicates presence of CGAL library])
	AC_SUBST(CGAL_CPPFLAGS)
	AC_SUBST(CGAL_LDFLAGS)
        # execute ACTION-IF-FOUND
        ifelse([$1], , :, [$1])
else
        # execute ACTION-IF-NOT-FOUND
        ifelse([$2], , :, [$2])
fi

])

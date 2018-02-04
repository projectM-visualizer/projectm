# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_check_gd.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_GD
#
# DESCRIPTION
#
#   Check for the gd library. (See http://www.boutell.com/gd/) If gd is
#   found, the output variables GD_CFLAGS, GD_LDFLAGS and GD_LIBS will
#   contain the compiler flags, linker flags and libraries necessary to use
#   gd; otherwise, those variables will be empty. In addition, the symbol
#   HAVE_GD is defined if the library is found, and the symbols HAVE_GD_GIF,
#   HAVE_GD_JPEG and HAVE_GD_PNG are defined if the library supports
#   creating images in gif, jpeg and png formats, respectively.
#
#   The user may use --with-gd=no or --without-gd to skip checking for the
#   library. (The default is --with-gd=yes.) If the library is installed in
#   an unusual location, --with-gd=DIR will cause the macro to look for
#   gdlib-config in DIR/bin or, failing that, for the headers and libraries
#   in DIR/include and DIR/lib.
#
#   Feedback welcome!
#
# LICENSE
#
#   Copyright (c) 2008 Nick Markham <markhn@rpi.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 11

AC_DEFUN([AX_CHECK_GD], [
	AC_ARG_WITH(gd,
		AS_HELP_STRING([--with-gd(=DIR)], [use the gd library (in DIR)]),,
		with_gd=yes)

	if test "$with_gd" != no; then
		AC_PATH_PROG(GDLIB_CONFIG, gdlib-config, , [$with_gd/bin:$PATH])
		if test -n "$GDLIB_CONFIG"; then
			GD_CFLAGS=`$GDLIB_CONFIG --cflags`
			GD_LDFLAGS=`$GDLIB_CONFIG --ldflags`
			GD_LIBS=`$GDLIB_CONFIG --libs`
		elif test -d "$with_gd"; then
			GD_CFLAGS="-I$with_gd/include"
			GD_LDFLAGS="-L$with_gd/lib"
			AC_CHECK_LIB(z, inflateReset, GD_LIBS="-lz")
			AC_CHECK_LIB(png, png_check_sig, GD_LIBS="-lpng $GD_LIBS", , $GD_LIBS)
		fi

		save_CFLAGS="$CFLAGS"
		CFLAGS="$GD_CFLAGS $CFLAGS"
		save_LDFLAGS="$LDFLAGS"
		LDFLAGS="$GD_LDFLAGS $LDFLAGS"

		AC_CHECK_LIB(gd, gdImageCreate, [
			AC_DEFINE(HAVE_GD, 1, [ Define if you have gd library. ])
			AC_CHECK_LIB(gd, gdImageGif, AC_DEFINE(HAVE_GD_GIF, 1, [ Define if GD supports gif. ]), , "$GD_LIBS")
			AC_CHECK_LIB(gd, gdImageJpeg, AC_DEFINE(HAVE_GD_JPEG, 1, [ Define if GD supports jpeg. ]), , "$GD_LIBS")
			AC_CHECK_LIB(gd, gdImagePng, AC_DEFINE(HAVE_GD_PNG, 1, [ Define if GD supports png. ]), , "$GD_LIBS")
			GD_LIBS="-lgd $GD_LIBS"
		], with_gd=no, $GD_LIBS)

		CFLAGS="$save_CFLAGS"
		LDFLAGS="$save_LDFLAGS"
	fi

	if test "$with_gd" = "no"; then
		GD_CFLAGS="";
		GD_LDFLAGS="";
		GD_LIBS="";
	fi

	AC_SUBST(GD_CFLAGS)
	AC_SUBST(GD_LDFLAGS)
	AC_SUBST(GD_LIBS)
])

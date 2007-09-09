# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(jack-projectM plugin, 1.00, none , jack-projectM)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AC_PREFIX_DEFAULT(/usr/local)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LN_S

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PATH_PROG(PKG_CONFIG, [pkg-config], [no])
if test x$PKG_CONFIG = xno ; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/])
fi
if $PKG_CONFIG --atleast-pkgconfig-version 0.14 ; then
  :
else
  AC_MSG_ERROR([*** pkg-config too old; version 0.14 or better required.])
fi

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([GL/gl.h GL/glx.h GL/glu.h],opengl="yes",opengl="no")
if test x"$opengl" = xno; then
	AC_MSG_ERROR([OpenGL and Glu headers not found, the plugin cannot be build])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE # reserved for future

# Check for libraries
PKG_CHECK_MODULES(JACK, jack >= 0.0.0, [libjack="yes"], [libjack="no"])
if test x$libjack = xno; then
	AC_MSG_ERROR([*** libjack not found.
			libjack is needed to build this packag])
fi
AC_SUBST(JACK_LIBS)
AC_SUBST(JACK_CFLAGS)

PKG_CHECK_MODULES(LIBPROJECTM, libprojectM >= 0.0.0, [libprjM="yes"], [libprjM="no"])
if test x$libprjM = xno; then
	AC_MSG_ERROR([*** libprojectM not found.
			libprojectM is needed to build this package.
			You can download libprojectM at http://xmms-projectM.sf.net/])
fi
AC_SUBST(LIBPROJECTM_LIBS)
AC_SUBST(LIBPROJECTM_CFLAGS)

AC_DEFINE_UNQUOTED([PROJECTM_SYSCONFDIR],"`pkg-config --variable=sysconfdir libprojectM`",[libprojectM configuration dir])
AC_DEFINE_UNQUOTED([PROJECTM_DATADIR],"`pkg-config --variable=pkgdatadir libprojectM`",[libprojectM data dir])

AC_PATH_PROG(SDL_CONFIG, [sdl-config], [no])
if test x$SDL_CONFIG = xno ; then
  AC_MSG_ERROR([
*** sdl-config not found.
*** If you have installed from binaries, probably you have not 
*** installed SDL development package.
*** See http://www.libsdl.org
])
fi
AM_PATH_SDL(1.2.5, , AC_MSG_ERROR([*** SDL >= 1.2.5 not installed - please install first ***]))

# Internationalization
ALL_LINGUAS="es es_AR pt_BR nl fr ru"
GETTEXT_PACKAGE=jack-projectM
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

#
# Build paramaters
#

DEBUG_CFLAGS=""

AC_ARG_ENABLE([profiling],
  AC_HELP_STRING([--enable-profiling],
		[Enable emision of profile data to be used by gprof @<:@default=disabled@:>@]),
		[profiling=$enableval],
		[profiling=no])
AC_MSG_CHECKING([wheter to enable profiling])
if test x$profiling = xyes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="$DEBUG_CFLAGS -pg"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([debug],
            AC_HELP_STRING([--enable-debug],
                           [Enable debug @<:@default=disabled@:>@]),
	    [debug=$enableval],
	    [debug=no])
AC_MSG_CHECKING([whether to enable debug])
if test "$debug" = yes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="$DEBUG_CFLAGS -ggdb3"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wall -Wmissing-braces -Wimplicit -Wunused"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wmissing-prototypes -Wno-unused-variable"
else
  AC_MSG_RESULT([no])
fi
AC_SUBST(DEBUG_CFLAGS)

AC_ARG_ENABLE([extra-optimization],
              AC_HELP_STRING([--enable-extra-optimization],
                             [Enable extra optimizations @<:@default=disabled@:>@]),
              [extra_opt=$enableval],
              [extra_opt=no])
AC_MSG_CHECKING([wheter to enable extra optimizations])
if test x$extra_opt = xyes; then
  AC_MSG_RESULT([yes])
  OPT_CFLAGS="-O3 -fexpensive-optimizations"
else
  AC_MSG_RESULT([no])
  if test x$debug = xyes; then
	OPT_CFLAGS="-O0"
  else
	OPT_CFLAGS=""
  fi
fi
AC_SUBST(OPT_CFLAGS)

AC_SUBST(CFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS}")

# cflags, ld flags

libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_SUBST(BUILD_INPUT, "$build_input")
AC_SUBST(BUILD_ACTOR, "$build_actor")

AC_CONFIG_FILES([
	Makefile
	])

AC_OUTPUT

echo "
=====================================================================
jack-projectM, xmms plugin for projectM $VERSION
=====================================================================

Packages found:

opengl:                               : ${opengl}

Build options:
prefix:                               : ${prefix}
source code location:                 : ${srcdir}
compiler:                             : ${CC}
cflags:                               : ${CFLAGS}
"


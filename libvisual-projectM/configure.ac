# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual projectM, 0.98, psperl@sourceforge.net)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
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

# Checks for libraries.
build_actor=""


PKG_CHECK_MODULES(LIBPROJECTM, libprojectM >= 0.0.0, [libprjM="yes"], [libprjM="no"])
if test x$libprjM = xno; then
	AC_MSG_ERROR([*** libprojectM not found.
			libprojectM is needed to build this package.
			You can download libprojectM at http://xmms-projectM.sf.net/])
fi
AC_SUBST(LIBPROJECTM_LIBS)
AC_SUBST(LIBPROJECTM_CFLAGS)

 # define some paths macros about libprojectM
AC_DEFINE_UNQUOTED([PROJECTM_SYSCONFDIR],"`pkg-config --variable=sysconfdir libprojectM`",[libprojectM configuration dir])
AC_DEFINE_UNQUOTED([PROJECTM_DATADIR],"`pkg-config --variable=pkgdatadir libprojectM`",[libprojectM data dir])


PKG_CHECK_MODULES(LIBVISUAL, libvisual-0.4 >= 0.4.0, [libvis="yes"], [libvis="no"])
if test x$libvis = xno; then
	AC_MSG_ERROR([*** libvisual not found.
			libvisual is needed to build this package.
			You can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)

PKG_CHECK_MODULES(FTGL, ftgl >= 2.0.0, [libft="yes"], [libft="no"])
if test x$libft = xno; then
	AC_MSG_ERROR([*** ftgl not found.
			libftgl is needed to build this package./])
fi
AC_SUBST(FTGL_LIBS)
AC_SUBST(FTGL_CFLAGS)

AC_PATH_X

opengl=no
if test x$have_x = xyes ; then

    test x$x_libraries != x && LDFLAGS="$LDFLAGS -Wl,-rpath,$x_libraries -L$x_libraries"
    test x$x_includes != x && CPPFLAGS="$CPPFLAGS -I$x_includes"

    AC_CHECK_HEADERS([\
	X11/keysym.h	\
	X11/X.h	\
	X11/Xlib.h	\
	X11/Xutil.h], [], [
	    AC_MSG_ERROR([Required X11 headers not found.])
	])

    AC_CHECK_HEADERS([\
	GL/gl.h \
	GL/glx.h], [have_gl_headers=yes], [have_gl_headers=no])

    if test x$have_gl_headers = xyes ; then
	AC_CHECK_LIB([GL], [glBegin], [have_lib_gl=yes], [have_lib_gl=no])
    else
	have_lib_gl=no
    fi

    opengl=$have_lib_gl

    AM_CONDITIONAL(HAVE_LIB_GL,  test x$have_lib_gl = xyes)

    XF86VM_LIB=
    AC_CHECK_HEADERS([\
	X11/extensions/xf86vmode.h], [
	    AC_CHECK_LIB([Xxf86vm], [XF86VidModeSwitchToMode], [
		AC_DEFINE(HAVE_XF86VMODE, [], [XF86VidMode is available])
		XF86VM_LIB=-lXxf86vm
		xf86vm=yes
	    ], [xf86vm=no]) ], [],
	    [[#include <X11/Xlib.h>
	    ]])

    AC_SUBST(XF86VM_LIB)
    AM_CONDITIONAL(HAVE_XF86VMODE,  test x$xf86vm = xyes)
else
    opengl=no
fi

AM_CONDITIONAL(HAVE_X,  test x$have_x = xyes)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
# Checks for header files.
AC_HEADER_TIME
AC_CHECK_HEADERS([fcntl.h unistd.h stdlib.h string.h sys/time.h])
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memmove memset pow sqrt])
AC_CHECK_FUNC([munmap], lv_have_munmap=yes, lv_have_munmap=no)
AC_FUNC_MMAP

AC_ARG_ENABLE([projectm],
            AC_HELP_STRING([--disable-projectm],
                           [Do not build projectm plugin @<:@default=enabled@:>@]),
	    [lvprojectm=$enableval],
	    [lvprojectm=yes])
AC_MSG_CHECKING([whether to build lv_projectm plugin])
if test x$lvprojectm = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor actor_projectm"
    AC_SUBST([PROJECTM], ['actor_projectM.la'])
  else
    AC_MSG_WARN([*** No OpenGL found.
		projectm plugin will not be built.])
    build_actor="$build_actor actor_projectm"
    AC_SUBST([PROJECTM], ['actor_projectM.la'])
  fi
else
  AC_MSG_RESULT([no])
fi

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
AC_SUBST(CXXFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS}")

AC_ARG_WITH([plugins_base_dir],
            AC_HELP_STRING([--with-plugins_base_dir="dir"],
                           [Install plugins to this base directory. Defaults to Libvisual's base dir ]),
dnl ' # fix string coloring ;)
	    [plugins_base_dir=$withval],
	    [plugins_base_dir=])

AC_MSG_CHECKING([where to install plugins])

if test -z "$plugins_base_dir"; then
    plugins_base_dir=`$PKG_CONFIG --variable=pluginsbasedir libvisual-0.4`
    AC_MSG_RESULT([$plugins_base_dir])
else
    if test -d "$plugins_base_dir"; then
        AC_MSG_RESULT([$plugins_base_dir])
    else
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Specified plugins base directory "$plugins_base_dir" is not accesible!])
        exit 1
    fi
fi

LIBVISUAL_PLUGINS_BASE_DIR="$plugins_base_dir"
AC_SUBST(LIBVISUAL_PLUGINS_BASE_DIR)

libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_SUBST(DATADIR, "$datadir")

AC_CONFIG_FILES([
	Makefile    
])


AC_OUTPUT

echo "
================================================
LIBVISUAL-PROJECTM, THE PROJECTM PLUGIN FOR LIBVISUAL $VERSION
================================================

Packages found:
libvisual:                            : ${libvis}

Plugins being build:
actor:                                : ${build_actor}

Libvisual plugin data dir:            : ${prefix}/share

Build options:
install path                          : $plugins_base_dir
source code location:                 : ${srcdir}
compiler:                             : ${CC}
"


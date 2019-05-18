AC_INIT([projectM], [3.1.1-rc4], [mischa@mvstg.biz], [projectM], [https://github.com/projectM-visualizer/projectm/])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects tar-pax])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT

# Check if we should disable rpath.
#
# For advanced users: In certain configurations, the rpath attributes
# added by libtool cause problems as rpath will be preferred over
# LD_LIBRARY_PATH. This does not seem to be a problem with
# clang. When using --disable-rpath you will likely need to set
# LD_LIBRARY_PATH if you are using libraries in non-system locations.
# YMMV.
#

DISABLE_RPATH

AC_PROG_CXX
AC_LANG(C++)

AC_CONFIG_MACRO_DIRS([m4 m4/autoconf-archive])

dnl emscripten
AC_ARG_ENABLE([emscripten],
AS_HELP_STRING([--enable-emscripten], [Build for web with emscripten]),
[], [enable_emscripten=no])
AS_IF([test "x$enable_emscripten" = "xyes" || test "x$EMSCRIPTEN" = "xyes"], [
  dnl Set up emscripten
  m4_include([m4/emscripten.m4])
  AC_DEFINE([EMSCRIPTEN], [1], [Define EMSCRIPTEN])
  enable_threading=no
  enable_gles=yes
  enable_sdl=yes
], [
  dnl Running in a normal OS (not emscripten)
  AX_CHECK_GL

  # check OS
  AC_CANONICAL_HOST
  AC_MSG_CHECKING(Freedom)
  case $host_os in
  darwin*)
    # OSX needs CoreFoundation
    AC_MSG_RESULT(Apple hoarderware detected)
    LIBS="$LIBS -framework CoreFoundation"
    ;;
  linux*)
    # limux needs dl
    AC_MSG_RESULT(GNU/LINUX detected)
    LIBS="$LIBS -ldl"
    ;;
  esac
])


AC_CHECK_LIB(c, dlopen, LIBDL="", AC_CHECK_LIB(dl, dlopen, LIBDL="-ldl"))

AC_CHECK_FUNCS_ONCE([aligned_alloc posix_memalign])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/libprojectM/Makefile
  src/libprojectM/Renderer/Makefile
  src/libprojectM/NativePresetFactory/Makefile
  src/libprojectM/MilkdropPresetFactory/Makefile
  src/libprojectM/libprojectM.pc
  src/NativePresets/Makefile
  src/projectM-sdl/Makefile
  src/projectM-emscripten/Makefile
  src/projectM-qt/Makefile
  src/projectM-pulseaudio/Makefile
  src/projectM-jack/Makefile
  src/projectM-test/Makefile
  src/projectM-gst/Makefile
  src/projectM-gst/element/Makefile
])


# SDL
AC_ARG_ENABLE([sdl], AS_HELP_STRING([--enable-sdl], [Build SDL2 application]), [], [enable_sdl=check])
AS_IF([test "$enable_sdl" != "no"], [
    PKG_CHECK_MODULES([SDL], [sdl2], [
        m4_include([m4/sdl2.m4])
        SDL_VERSION="2.0.5"
        AS_IF([test "$TRAVIS"], [SDL_VERSION=2.0.2])  # travis has old SDL, we don't care
        AS_IF([test "$EMSCRIPTEN"], [SDL_VERSION=2.0.0])  # emscripten has old SDL, we don't care

        # Check for libSDL >= $SDL_VERSION
        AM_PATH_SDL2($SDL_VERSION,
            [enable_sdl=yes],
            [AS_IF([test "$enable_sdl" = "yes"], AC_MSG_ERROR([*** SDL version >= $SDL_VERSION not found!])); enable_sdl=no])
    ],
    [
        # not found
        AS_IF([test "$enable_sdl" = "yes"], AC_MSG_ERROR([*** libsdl2 not found!]))
        enable_sdl=no
    ])
])


# glm
AS_IF([test "x$enable_emscripten" != "xyes"], [
  AC_CHECK_HEADER([glm/glm.hpp],, AC_MSG_ERROR(libglm is required.))
])


# Threading
AC_ARG_ENABLE([threading],
AS_HELP_STRING([--enable-threading], [multhreading]),
[], [enable_threading=yes])
AS_IF([test "x$enable_threading" = "xyes" && ! test "$EMSCRIPTEN"], [
  m4_include([m4/autoconf-archive/ax_pthread.m4])
  AX_PTHREAD([
    AC_DEFINE([USE_THREADS], [1], [Define USE_THREADS])
    LIBS="$LIBS $PTHREAD_LIBS $PTHREAD_CFLAGS"
    CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
    CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
    echo "LIBS=$LIBS"
  ], [
    AC_MSG_ERROR([pthreads not found])
  ])
])

AC_ARG_ENABLE([gles],
  AS_HELP_STRING([--enable-gles], [OpenGL ES support]),
  [], [enable_gles=no])
  AS_IF([test "x$enable_gles" = "xyes"], [
    AC_DEFINE([USE_GLES], [1], [Define USE_GLES])
])

dnl LLVM
dnl unfortuately AX_LLVM macro seems to be out of date, so we're going to rely on the user to make sure LLVM is installed correctly
AC_ARG_ENABLE([llvm],
  AS_HELP_STRING([--enable-llvm],[Support for JIT using LLVM]),
  [], [enable_llvm=no])
AS_IF([test x"$enable_llvm" = "xyes"], [
    AC_DEFINE([HAVE_LLVM], [1], [Define HAVE_LLVM])
    CFLAGS="$CFLAGS -I$(llvm-config --includedir)"
    CXXFLAGS="$CXXFLAGS -I$(llvm-config --includedir)"
    LIBS="$LIBS $(llvm-config --libs)"
    LDFLAGS="$LDFLAGS $(llvm-config --ldflags)"
])

dnl from https://stackoverflow.com/questions/30897170/ac-subst-does-not-expand-variable answer: https://stackoverflow.com/a/30960268
dnl ptomato https://stackoverflow.com/users/172999/ptomato
AC_SUBST([PACKAGE])
AC_PROG_SED
AC_CONFIG_FILES([src/libprojectM/config.inp.in])

AC_PREFIX_DEFAULT([/usr/local])

AC_PROG_MKDIR_P

AS_IF([grep android <<< ${host}], [],
  [AX_CHECK_COMPILE_FLAG([-stdlib=libc++], [
                        CXXFLAGS="$CXXFLAGS -stdlib=libc++"])
])

AX_CHECK_COMPILE_FLAG([-std=c++11], [
                        CXXFLAGS="$CXXFLAGS -std=c++11"])

# Qt5
AC_ARG_ENABLE([qt], AS_HELP_STRING([--enable-qt], [Enable Qt: needed for pulseaudio and jack GUIs]), [], [enable_qt=check])
AS_IF([test "$enable_qt" != "no"],
  [
    AX_HAVE_QT  # m4/qt.m4

    AS_IF([test "$have_qt" = "yes"], [
        # we have at least qt5 if $have_qt is true
        enable_qt=yes
        export QT_SELECT=qt5
    ],
    [AS_IF([test "$enable_qt" = "yes"],
        [AC_MSG_ERROR(["Qt5 not found"])],
        [enable_qt=no])]
)])


# Pulseaudio
AC_ARG_ENABLE([pulseaudio], AS_HELP_STRING([--enable-pulseaudio], [Build Pulseaudio]), [], [enable_pulseaudio=check])
AS_IF([test "$enable_pulseaudio" != "no"],
    [PKG_CHECK_MODULES([libpulse],
        [libpulse],
        [
            # still need qt
            AS_IF([test "$enable_qt" = "yes"],
                [enable_pulseaudio=yes],
                [enable_pulseaudio="Qt required"])
        ],
        [AS_IF([test "$enable_pulseaudio" = "yes"],
            [AC_MSG_ERROR([libpulse required, but not found.])],
            [enable_pulseaudio=no])])])


# Jack
AC_ARG_ENABLE([jack], AS_HELP_STRING([--enable-jack], [Build Jack]), [], [enable_jack=check])
AS_IF([test "$enable_jack" != "no"],
    [PKG_CHECK_MODULES([jack],
    [jack],
    [
        # still need qt
        AS_IF([test "$enable_qt" = "yes"],
            [enable_jack=yes],
            [enable_jack="Qt required"])
    ],
    [AS_IF([test "$enable_jack" = "yes"],
        [AC_MSG_ERROR([jack required, but not found.])],
        [enable_jack=no])])])


AC_ARG_ENABLE([gst], AS_HELP_STRING([--enable-gst], [Build GStreamer]), [], [enable_gst=check])
AS_IF([test "$enable_gst" != "no"],
    [PKG_CHECK_MODULES([GST],
    [gstreamer-1.0 gstreamer-audio-1.0],
    [
      enable_gst=yes

      dnl set the plugindir where plugins should be installed (for plugins/Makefile.am)
      if test "x${prefix}" = "x$HOME"; then
         gstplugindir="$HOME/.gstreamer-1.0/plugins"
      else
         gstplugindir="\$(libdir)/gstreamer-1.0"
      fi
      AC_SUBST(gstplugindir)

      dnl set proper LDFLAGS for plugins
      GST_PLUGIN_LDFLAGS='-module -avoid-version -export-symbols-regex [_]*\(gst_\|Gst\|GST_\).*'
      AC_SUBST(GST_PLUGIN_LDFLAGS)
    ],
    [AS_IF([test "$enable_gst" = "yes"],
        [AC_MSG_ERROR([gstreamer-1.0 required, but not found.])],
        [enable_gst=no])])])


AM_CONDITIONAL([ENABLE_SDL], [test "x$enable_sdl" = "xyes"])
AM_CONDITIONAL([ENABLE_QT], [test "x$enable_qt" = "xyes"])
AM_CONDITIONAL([ENABLE_JACK], [test "x$enable_jack" = "xyes"])
AM_CONDITIONAL([ENABLE_PULSEAUDIO], [test "x$enable_pulseaudio" = "xyes"])
AM_CONDITIONAL([ENABLE_GST], [test "x$enable_gst" = "xyes"])
AM_CONDITIONAL([ENABLE_EMSCRIPTEN], [test "x$enable_emscripten" = "xyes"])


my_CFLAGS="-Wall -Wchar-subscripts -Wformat-security -Wpointer-arith -Wshadow -Wsign-compare -Wtype-limits "
#my_CFLAGS+="-fsanitize=address -fno-omit-frame-pointer "
my_CFLAGS+='-DDATADIR_PATH=\""$(pkgdatadir)"\" '
my_CFLAGS+='-I$(top_srcdir)/vendor '
AC_SUBST([my_CFLAGS])



AC_OUTPUT

AC_MSG_RESULT([
projectM v$VERSION
=====

prefix:                 ${prefix}
sysconfdir:             ${sysconfdir}
libdir:                 ${libdir}
includedir:             ${includedir}

compiler:               ${CC}
cflags:                 ${CFLAGS} ${my_CFLAGS}
cxxflags:               ${CXXFLAGS}
libs:                   ${LIBS}
ldflags:                ${LDFLAGS}

- - -

Applications:
=====

libprojectM:            yes
Threading:              ${enable_threading}
SDL:                    ${enable_sdl}
Qt:                     ${enable_qt}
Pulseaudio:             ${enable_pulseaudio}
Jack:                   ${enable_jack}
OpenGLES:               ${enable_gles}
Emscripten:             ${enable_emscripten}
llvm:                   ${enable_llvm}
gstreamer:              ${enable_gst}
])

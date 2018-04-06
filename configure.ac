AC_INIT([projectM], [2.2.1], [mischa@mvstg.biz], [projectM], [https://github.com/revmischa/projectm])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects tar-pax])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT

AC_PROG_CXX

AC_CONFIG_MACRO_DIRS([m4 m4/autoconf-archive])
AX_CHECK_GL

AC_CHECK_LIB(c, dlopen, LIBDL="", AC_CHECK_LIB(dl, dlopen, LIBDL="-ldl"))

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/libprojectM/Makefile
  src/libprojectM/Renderer/Makefile
  src/libprojectM/NativePresetFactory/Makefile
  src/libprojectM/MilkdropPresetFactory/Makefile
  src/NativePresets/Makefile
  src/projectM-sdl/Makefile
  src/projectM-qt/Makefile
  src/projectM-pulseaudio/Makefile
])

dnl SDL
AC_ARG_ENABLE([sdl],
  AS_HELP_STRING([--enable-sdl], [Build SDL2 app]),
  [], [enable_sdl=no])
AS_IF([test "x$enable_sdl" = "xyes"], [
  m4_include([m4/sdl2.m4])
  SDL_VERSION=2.0.5
  AS_IF([test "$TRAVIS"], [SDL_VERSION=2.0.2])  # travis has old SDL, we don't care
  AM_PATH_SDL2($SDL_VERSION, :, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
])

dnl FTGL
AC_ARG_ENABLE([ftgl],
  AS_HELP_STRING([--enable-ftgl], [FTGL support to display text such as preset title, stats, fps, ...]),
  [], [enable_ftgl=no])
AS_IF([test "x$enable_ftgl" = "xyes"], [
  PKG_CHECK_MODULES(FTGL, [ftgl], [], [AC_MSG_ERROR([libftgl is required.])])
  AC_DEFINE([USE_FTGL], [1], [Define USE_FTGL])
])

dnl from https://stackoverflow.com/questions/30897170/ac-subst-does-not-expand-variable answer: https://stackoverflow.com/a/30960268
dnl ptomato https://stackoverflow.com/users/172999/ptomato
AC_SUBST([PACKAGE])
AC_PROG_SED
AC_CONFIG_FILES([src/libprojectM/config.inp.in])

AC_PREFIX_DEFAULT([/usr/local])

AC_PROG_MKDIR_P

dnl Qt
AC_ARG_ENABLE([qt],
  AS_HELP_STRING([--enable-qt], [Build Qt]),
  [], [enable_qt=no])
AS_IF([test "x$enable_qt" = "xyes"], [
  PKG_CHECK_MODULES(QT, [Qt5Core, Qt5Gui, Qt5Widgets Qt5OpenGL], [], [AC_MSG_ERROR([Qt libraries are required.])])
  qt_CPPFLAGS="`$PKG_CONFIG --cflags-only-I Qt5Core Qt5Gui Qt5Widgets Qt5OpenGL` $CPPFLAGS"
  qt_LDFLAGS="`$PKG_CONFIG --libs-only-L Qt5Core Qt5Gui Qt5Widgets Qt5OpenGL` $LDFLAGS"
  qt_LIBS="`$PKG_CONFIG --libs-only-l Qt5Core Qt5Gui Qt5Widgets Qt5OpenGL` $LIBS"

  if ! `$PKG_CONFIG --atleast-version=5.0.0 Qt5Core`; then
    AC_MSG_ERROR([Qt >= 5.0.0 is required. Try installing qtdeclarative5-dev])
  fi

  AC_CHECK_PROGS(MOC, [moc-qt5 moc])
  AC_CHECK_PROGS(UIC, [uic-qt5 uic])
  AC_CHECK_PROGS(RCC, [rcc])
  if test -z "$MOC" || test -z "$UIC" || test -z "$RCC"; then
    AC_MSG_ERROR([Qt utility programs moc, uic, and rcc are required.])
  fi

  PKG_CHECK_MODULES(LIBPULSE, [libpulse], [], [AC_MSG_ERROR([Pulseaudio library libpulse is required.])])
])

AM_CONDITIONAL([ENABLE_SDL], [test "$enable_sdl" = yes])
AM_CONDITIONAL([ENABLE_QT], [test "$enable_qt" = yes])


my_CFLAGS="-Wall -Wchar-subscripts -Wformat-security -Wmissing-declarations -Wpointer-arith -Wshadow -Wsign-compare -Wtype-limits "
AC_SUBST([my_CFLAGS])

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



AC_OUTPUT

AC_MSG_RESULT([
projectM v$VERSION
=====

prefix:                 ${prefix}
sysconfdir:             ${sysconfdir}
libdir:                 ${libdir}
includedir:             ${includedir}

compiler:               ${CC}
cflags:                 ${CFLAGS}
ldflags:                ${LDFLAGS}

- - -

Applications:
=====

libprojectM:            yes
SDL:                    ${enable_sdl}
Qt & Pulseaudio:        ${enable_qt}
FTGL:                   ${enable_ftgl}
])

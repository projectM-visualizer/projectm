AC_INIT([projectM], [2.2.0], [mischa@mvstg.biz], [projectM], [https://github.com/revmischa/projectm])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects tar-pax])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT

AC_PROG_CXX

AC_CONFIG_MACRO_DIRS([m4 m4/autoconf-archive])
AX_CHECK_GL

AC_CHECK_LIB(c, dlopen, LIBDL="", AC_CHECK_LIB(dl, dlopen, LIBDL="-ldl"))

dnl SDL
AC_ARG_ENABLE([sdl],
  AS_HELP_STRING([--enable-sdl], [Build SDL2 app]),
  [], [enable_sdl=no])
AS_IF([test "x$enable_sdl" = "xyes"], [
  m4_include([m4/sdl2.m4])
  SDL_VERSION=2.0.5
  AS_IF([test "$TRAVIS"], [SDL_VERSION=2.0.2])  # travis has old SDL, we don't care
  AM_PATH_SDL2($SDL_VERSION, :, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
  AC_CONFIG_FILES([src/projectM-sdl/Makefile])
])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/libprojectM/Makefile
  src/libprojectM/Renderer/Makefile
  src/libprojectM/NativePresetFactory/Makefile
  src/libprojectM/MilkdropPresetFactory/Makefile
])
dnl FIXME
dnl AC_CONFIG_FILES([src/libprojectM/config.inp],
dnl [cat src/libprojectM/config.inp | sed -e"s,\${prefix},$prefix," > src/libprojectM/config.inp.b])

AC_PREFIX_DEFAULT([/usr/local])

AC_PROG_MKDIR_P

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


Applications:
=====

SDL:                    ${enable_sdl}
])

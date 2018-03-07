AC_INIT([projectM], [2.2.0], [mischa@mvstg.biz], [projectM], [https://github.com/revmischa/projectm])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects tar-pax])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT

AC_PROG_CXX

AC_CONFIG_MACRO_DIRS([m4 m4/autoconf-archive])
m4_include([m4/sdl2.m4])
AX_CHECK_GL

AC_CHECK_LIB(c, dlopen, LIBDL="", AC_CHECK_LIB(dl, dlopen, LIBDL="-ldl"))

dnl Check for SDL
SDL_VERSION=2.0.5
AS_IF([test "$TRAVIS"], [SDL_VERSION=2.0.2])  # travis has old SDL, we don't care
AM_PATH_SDL2($SDL_VERSION, :, AC_MSG_WARN([*** SDL version $SDL_VERSION not found!]))

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/libprojectM/Makefile
  src/libprojectM/Renderer/Makefile
  src/libprojectM/NativePresetFactory/Makefile
  src/libprojectM/MilkdropPresetFactory/Makefile
  src/projectM-sdl/Makefile
])
AC_CONFIG_FILES([src/libprojectM/config.inp], [sed -i -e"s/\${prefix}/$prefix/" src/libprojectM/config.inp])

AC_PREFIX_DEFAULT([/usr/local])

AC_PROG_MKDIR_P

AC_ARG_ENABLE([logging],
AS_HELP_STRING([--disable-logging], [disable system logging @<:@default=enabled@:>@]),
[], enable_logging=yes)
AS_IF([test "x$enable_logging" = "xyes"], [
AC_DEFINE(ENABLE_LOGGING, [1], [System logging.])
])

AC_ARG_ENABLE([debug],
AS_HELP_STRING([--enable-debug], [enable debug messages @<:@default=disabled@:>@]),
[], [enable_debug=no])
AS_IF([test "x$enable_debug" = "xyes"], [
AC_DEFINE(ENABLE_DEBUG, [1], [Debug messages.])
])

my_CFLAGS="\
-Wall \
-Wchar-subscripts \
-Wformat-security \
-Wmissing-declarations \
-Wpointer-arith \
-Wshadow \
-Wsign-compare \
-Wtype-limits \
"
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

logging:                ${enable_logging}
debug:                  ${enable_debug}
])

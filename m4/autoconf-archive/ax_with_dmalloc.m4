# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_with_dmalloc.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_WITH_DMALLOC
#
# DESCRIPTION
#
#   Let the user enable/disable support for the dmalloc library available
#   from <http://www.dmalloc.org/>.
#
#   The macro adds the command-line flag "--with-dmalloc". Furthermore,
#   "-IPREFIX/include" will be added to "$CPPFLAGS", "-LPREFIX/lib" to
#   "$LDFLAGS", and "-DDEBUG_DMALLOC" and "-DDMALLOC_FUNC_CHECK" to
#   "$CPPFLAGS".
#
#   To enable dmalloc support in your code, add the following snippet to
#   your header files:
#
#     #ifdef DEBUG_DMALLOC
#     #  include <dmalloc.h>
#     #endif
#
# LICENSE
#
#   Copyright (c) 2008 Peter Simons <simons@cryp.to>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_WITH_DMALLOC], [
AC_MSG_CHECKING(whether to use the dmalloc library)
AC_ARG_WITH(dmalloc,
[  --with-dmalloc[=PREFIX]  Compile with dmalloc library],
if test "$withval" = "" -o "$withval" = "yes"; then
    ac_cv_dmalloc="/usr/local"
else
    ac_cv_dmalloc="$withval"
fi
AC_MSG_RESULT(yes)
CPPFLAGS="$CPPFLAGS -DDEBUG_DMALLOC -DDMALLOC_FUNC_CHECK -I$ac_cv_dmalloc/include"
LDFLAGS="$LDFLAGS -L$ac_cv_dmalloc/lib"
LIBS="$LIBS -ldmalloc"
,AC_MSG_RESULT(no))
])dnl

# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_prog_javah.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_JAVAH
#
# DESCRIPTION
#
#   AX_PROG_JAVAH tests the availability of the javah header generator and
#   looks for the jni.h header file. If available, JAVAH is set to the full
#   path of javah and CPPFLAGS is updated accordingly.
#
# LICENSE
#
#   Copyright (c) 2008 Luc Maisonobe <luc@spaceroots.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AU_ALIAS([AC_PROG_JAVAH], [AX_PROG_JAVAH])
AC_DEFUN([AX_PROG_JAVAH],[
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
AC_PATH_PROG(JAVAH,javah)
AS_IF([test -n "$ac_cv_path_JAVAH"],
      [
        AC_TRY_CPP([#include <jni.h>],,[
        ac_save_CPPFLAGS="$CPPFLAGS"
		_ACJAVAH_FOLLOW_SYMLINKS("$ac_cv_path_JAVAH")
        ax_prog_javah_bin_dir=`AS_DIRNAME([$_ACJAVAH_FOLLOWED])`
        ac_dir="`AS_DIRNAME([$ax_prog_javah_bin_dir])`/include"
        AS_CASE([$build_os],
                [cygwin*|mingw*],
                [ac_machdep=win32],
                [ac_machdep=`AS_ECHO($build_os) | sed 's,[[-0-9]].*,,'`])
        CPPFLAGS="$ac_save_CPPFLAGS -I$ac_dir -I$ac_dir/$ac_machdep"
        AC_TRY_CPP([#include <jni.h>],
                   ac_save_CPPFLAGS="$CPPFLAGS",
                   AC_MSG_WARN([unable to include <jni.h>]))
        CPPFLAGS="$ac_save_CPPFLAGS"])
      ])
])

AC_DEFUN([_ACJAVAH_FOLLOW_SYMLINKS],[
# find the include directory relative to the javac executable
_cur="$1"
while ls -ld "$_cur" 2>/dev/null | grep " -> " >/dev/null; do
        AC_MSG_CHECKING([symlink for $_cur])
        _slink=`ls -ld "$_cur" | sed 's/.* -> //'`
        case "$_slink" in
        /*) _cur="$_slink";;
        # 'X' avoids triggering unwanted echo options.
        *) _cur=`echo "X$_cur" | sed -e 's/^X//' -e 's:[[^/]]*$::'`"$_slink";;
        esac
        AC_MSG_RESULT([$_cur])
done
_ACJAVAH_FOLLOWED="$_cur"
])

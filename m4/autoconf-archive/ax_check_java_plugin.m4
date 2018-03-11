# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_check_java_plugin.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_JAVA_PLUGIN(<shell-variable>)
#
# DESCRIPTION
#
#   This macro sets <shell-variable> to empty on failure and to a compatible
#   version of plugin.jar otherwise. Directories searched are /usr/java/*
#   and /usr/local/java/*, which are assumed to be j{dk,re} installations.
#   Apply the shell variable as you see fit. If sun changes things so
#   <jre>/lib/plugin.jar is not the magic file it will stop working.
#
#   This macro assumes that unzip, zipinfo or pkzipc is available (and can
#   list the contents of the jar archive). The first two are assumed to work
#   similarly enough to the infozip versions. The pkzipc version is assumed
#   to work if I understand the documentation on pkware's site but YMMV. I
#   do not have access to pwkware's version to test it.
#
# LICENSE
#
#   Copyright (c) 2008 Duncan Simpson <dps@simpson.demon.co.uk>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 11

AU_ALIAS([DPS_CHECK_PLUGIN], [AX_CHECK_JAVA_PLUGIN])
AC_DEFUN([AX_CHECK_JAVA_PLUGIN],
[AC_REQUIRE([AC_PROG_AWK])
AC_REQUIRE([AC_PROG_FGREP])
AC_CHECK_PROG(ZIPINFO,[zipinfo unzip pkzipc])
AC_MSG_CHECKING([for the java plugin])
case "x$ZIPINFO" in
[*/zipinfo)]
	zipinf="zipinfo -1" ;;
[*/unzip)]
	zipinf="unzip -l";;
[*/pkzipc)]
	ziping="unzipc -view";;
[x*)]
	AC_MSG_RESULT([skiped, none of zipinfo, unzip and pkzipc found])
	AC_SUBST($1,[])
	zipinf="";;
esac
if test "x$zipinf" != "x"; then
jplugin=""
for jhome in `ls -dr /usr/java/* /usr/local/java/* 2> /dev/null`; do
for jfile in lib/plugin.jar jre/lib/plugin.jar; do
if test "x$jplugin" = "x" && test -f "$jhome/$jfile"; then
eval "$zipinf $jhome/$jfile | $AWK '{ print \$NF; }' | $FGREP netscape/javascript/JSObject" >/dev/null 2>/dev/null
if test $? -eq 0; then
dnl Some version of gcj (and javac) refuse to work with some files
dnl that pass this test. To stop this problem make sure that the compiler
dnl still works with this jar file in the classpath
cat << \EOF > Test.java
/* [#]line __oline__ "configure" */
public class Test {
}
EOF
if eval "$JAVAC -classpath $jhome/$jfile Test.java 2>/dev/null >/dev/null" && test -f Test.class; then
jplugin="$jhome/$jfile"
fi
rm -f Test.java Test.class
fi; fi; done; done
if test "x$jplugin" != "x"; then
AC_SUBST($1,$jplugin)
AC_MSG_RESULT($jplugin)
else
AC_MSG_RESULT([java plugin not found])
AC_SUBST($1,[])
fi
fi
])

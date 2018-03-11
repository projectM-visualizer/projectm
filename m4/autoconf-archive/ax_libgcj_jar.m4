# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_libgcj_jar.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIBGCJ_JAR
#
# DESCRIPTION
#
#   Locate libgcj.jar so you can place it before everything else when using
#   gcj.
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

#serial 12

AU_ALIAS([DPS_LIBGCJ_JAR], [AX_LIBGCJ_JAR])
AC_DEFUN([AX_LIBGCJ_JAR],
[
AC_REQUIRE([AC_EXEEXT])
AC_REQUIRE([AX_PROG_JAVAC])
AC_REQUIRE([AC_PROG_FGREP])
AC_PROG_SED
if test "x$SED" = "x"; then
AC_MSG_WARN([sed not available, so libgcj.jar test skipped])
else
AC_MSG_CHECKING([if $JAVAC is gcj]);
jc=`eval "[echo x$JAVAC | $SED 's/^x.*\\/\\([^/]*\\)\$/x\\1/;s/^ *\\([^ ]*\\) .*$/\\1/;s/"$EXEEXT"$//']"`
if test "x$jc" != "xxgcj"; then
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(yes)
AC_MSG_CHECKING([libgcj.jar location])
save_cp="$CLASSPATH";
unset CLASSPATH;
AC_MSG_CHECKING([gcj default classpath])
cat << \EOF > Test.java
/* [#]line __oline__ "configure" */
public class Test {
}
EOF
lgcj=`eval "[$JAVAC -v -C Test.java 2>&1 | $FGREP \\(system\\) | $SED 's/^ *\\([^ ]*\\) .*$/\\1/;s/\\.jar\\//.jar/']"`;
if test -f Test.class && test "x$lgcj" != "x"; then
AC_MSG_RESULT($lgcj)
$1="$lgcj:"
else
AC_MSG_RESULT(failed)
$1=""
fi
if test "x$save_cp" != "x"; then CLASSPATH="$save_cp"; fi
rm -f Test.java Test.class
fi
fi
])

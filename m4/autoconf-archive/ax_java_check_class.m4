# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_java_check_class.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_JAVA_CHECK_CLASS(<class>,<action-if-found>,<action-if-not-found>)
#
# DESCRIPTION
#
#   Test if a Java class is available. Based on AX_PROG_JAVAC_WORKS. This
#   version uses a cache variable which is both compiler, options and
#   classpath dependent (so if you switch from javac to gcj it correctly
#   notices and redoes the test).
#
#   The macro tries to compile a minimal program importing <class>. Some
#   newer compilers moan about the failure to use this but fail or produce a
#   class file anyway. All moaning is sunk to /dev/null since I only wanted
#   to know if the class could be imported. This is a recommended followup
#   to AX_CHECK_JAVA_PLUGIN with classpath appropriately adjusted.
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

AU_ALIAS([DPS_JAVA_CHECK_CLASS], [AX_JAVA_CHECK_CLASS])
AC_DEFUN([AX_JAVA_CHECK_CLASS],[
m4_define([cache_val],[m4_translit(ax_cv_have_java_class_$1, " ." ,"__")])
if test "x$CLASSPATH" != "x"; then
xtra=" with classpath ${CLASSPATH}"
xopts=`echo ${CLASSPATH} | ${SED} 's/^ *://'`
xopts="-classpath $xopts"
else xtra=""; xopts=""; fi
cache_var="cache_val"AS_TR_SH([_Jc_${JAVAC}_Cp_${CLASSPATH}])
AC_CACHE_CHECK([if the $1 class is available$xtra], [$cache_var], [
JAVA_TEST=Test.java
CLASS_TEST=Test.class
cat << \EOF > $JAVA_TEST
/* [#]xline __oline__ "configure" */
import $1;
public class Test {
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS $xopts $JAVA_TEST) >/dev/null 2>&1; then
  eval "${cache_var}=yes"
else
  eval "${cache_var}=no"
  echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD
  cat $JAVA_TEST >&AS_MESSAGE_LOG_FD
fi
rm -f $JAVA_TEST $CLASS_TEST
])
if eval 'test "x$'${cache_var}'" = "xyes"'; then
$2
true; else
$3
false; fi])

# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_xtra_classpath.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_XTRA_CLASSPATH(<classpath>,<class>,<jarfile>,<action-if-found>,<action-if-not-found>)
#
# DESCRIPTION
#
#   Set $1 to extra classpath components required for class $2 found in a
#   jar file in $3. If the class is found do $4 and otherwise do $5. Uses
#   AX_JAVA_CHECK_CLASS for testing whether a class is available
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

AU_ALIAS([DPS_XTRA_CLASSPATH], [AX_XTRA_CLASSPATH])
AC_DEFUN([AX_XTRA_CLASSPATH],[
AC_PROG_SED

POTENTIAL_LOCATIONS="/usr/share/java"
for jhome in `ls -dr /usr/java/* /usr/local/java/* 2> /dev/null`; do
for jdir in lib jre/lib; do
POTENTIAL_LOCATIONS="$POTENTIAL_LOCATIONS $jhome/$jdir"
done; done

AX_JAVA_CHECK_CLASS([$2],[got="yes"],[got="no"])
cpxtra=""; saved_cp="${CLASSPATH}";
for location in $POTENTIAL_LOCATIONS; do
for jfile in $3; do
if test "x$got" != "xyes" && test -f "$location/$jfile"; then
CLASSPATH="${saved_cp}:$location/$jfile"
AX_JAVA_CHECK_CLASS([$2],[got="yes"; cpxtra="$location/$jfile:"],[got="no"])
fi; done; done;

if test "x${saved_cp}" != "x"; then
CLASSPATH="${saved_cp}"
else unset CLASSPATH; fi
if test "x$got" = "xyes"; then
$1="$cpxtra"
$4
true; else
$5
false; fi
])

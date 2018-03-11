# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_prog_java_cc.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_JAVA_CC
#
# DESCRIPTION
#
#   Finds the appropriate java compiler on your path. By preference the java
#   compiler is gcj, then jikes then javac.
#
#   The macro can take one argument specifying a space separated list of
#   java compiler names.
#
#   For example:
#
#     AX_PROG_JAVA_CC(javac, gcj)
#
#   The macro also sets the compiler options variable: JAVA_CC_OPTS to
#   something sensible:
#
#    - for GCJ it sets it to: @GCJ_OPTS@
#      (if GCJ_OPTS is not yet defined then it is set to "-C")
#
#    - no other compiler has applicable options yet
#
#   Here's an example configure.in:
#
#     AC_INIT(Makefile.in)
#     AX_PROG_JAVA_CC()
#     AC_OUTPUT(Makefile)
#     dnl End.
#
#   And here's the start of the Makefile.in:
#
#     PROJECT_ROOT      := @srcdir@
#     # Tool definitions.
#     JAVAC             := @JAVA_CC@
#     JAVAC_OPTS        := @JAVA_CC_OPTS@
#     JAR_TOOL          := @jar_tool@
#
# LICENSE
#
#   Copyright (c) 2008 Nic Ferrier <nferrier@tapsellferrier.co.uk>
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

#serial 5

# AX_PROG_JAVA_CC([COMPILER ...])
# --------------------------
# COMPILER ... is a space separated list of java compilers to search for.
# This just gives the user an opportunity to specify an alternative
# search list for the java compiler.
AU_ALIAS([AC_PROG_JAVA_CC], [AX_PROG_JAVA_CC])
AC_DEFUN([AX_PROG_JAVA_CC],
[AC_ARG_VAR([JAVA_CC],                [java compiler command])dnl
AC_ARG_VAR([JAVA_CC_FLAGS],           [java compiler flags])dnl
m4_ifval([$1],
      [AC_CHECK_TOOLS(JAVA_CC, [$1])],
[AC_CHECK_TOOL(JAVA_CC, gcj)
if test -z "$JAVA_CC"; then
  AC_CHECK_TOOL(JAVA_CC, javac)
fi
if test -z "$JAVA_CC"; then
  AC_CHECK_TOOL(JAVA_CC, jikes)
fi
])

if test "$JAVA_CC" = "gcj"; then
   if test "$GCJ_OPTS" = ""; then
      AC_SUBST(GCJ_OPTS,-C)
   fi
   AC_SUBST(JAVA_CC_OPTS, @GCJ_OPTS@,
        [Define the compilation options for GCJ])
fi
test -z "$JAVA_CC" && AC_MSG_ERROR([no acceptable java compiler found in \$PATH])
])# AX_PROG_JAVA_CC

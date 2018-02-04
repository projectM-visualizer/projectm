# ============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_spec_package_version.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_SPEC_PACKAGE_AND_VERSION ([specfile])
#   AX_SPEC_PACKAGE_NAME ([shellvar],[defaultvalue])
#   AX_SPEC_PACKAGE_VERSION ([shellvar],[defaultvalue])
#   AX_SPEC_PACKAGE_SUMMARY ([shellvar],[defaultvalue])
#   AX_SPEC_PACKAGE_LICENSE ([shellvar],[defaultvalue])
#   AX_SPEC_PACKAGE_CATEGORY ([shellvar],[defaultvalue])
#   AX_SPEC_PACKAGE_ICON ([shellvar],[defaultvalue])
#   AX_SPEC_DEFAULTS([specfile])
#
# DESCRIPTION
#
#   set PACKAGE from the given specfile - default to basename of the
#   rpmspecfile if no "name:" could be found in the spec file.
#
#   set VERSION from the given specfile - default to a date-derived value if
#   no "version:" could be found in the spec file.
#
#   this macro builds on top of AX_SPEC_FILE / AX_SPEC_EXTRACT
#
#   more specific: if not "name:" or "%define name" was found in the
#   myproject.spec file then the PACKAGE var is set to the basename
#   "myproject". When no spec file was present then it will usually default
#   to "TODO".
#
#   The version spec looks for "version:" or "%define version" in the spec
#   file. When no such value was seen or no spec file had been present then
#   the value is set to `date +0.%y.%W%w`.
#
#   the version value itself is sanitized somewhat with making it to always
#   carry at least three digits (1.2.3) and cleansing superfluous "0" chars
#   around from generating numbers elsewhere.
#
#   additional macros are provided that extract a specific value from the
#   spec file, among these:
#
#   set PACKAGE_SUMMARY from the given specfile - default to package and try
#   to detect a type suffix if "summary:" was not in the spec file
#
#   set PACKAGE_LICENSE from the given specfile - if no "license:" tag was
#   given in the spec file then various COPYING files are grepped to have a
#   guess and the final fallback will be GNU GPL (or GNU LGPL).
#
#   set PACKAGE_ICON from the given specfile - if no "icon:" tag was given
#   in the spec file then we default to $PACKAGE-icon.png
#
#   the final AX_SPEC_INIT(specfile) will initialize all variables to its
#   defaults according to the spec file given.
#
# LICENSE
#
#   Copyright (c) 2008 Guido U. Draheim <guidod@gmx.de>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 3 of the License, or (at your
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

AC_DEFUN([AX_SPEC_PACKAGE_LICENSE],[AC_REQUIRE([AX_SPEC_FILE])dnl
  AS_VAR_PUSHDEF([VAR],[PACKAGE_LICENSE])dnl
  AC_MSG_CHECKING([for spec license type])
  if test ".$VAR" = "." ; then if test ! -f $ax_spec_file
  then k="(w/o spec)"
  else k=""
    AX_SPEC_EXTRACT(VAR,[license],m4_ifval($1,$1))
    VAR=`echo $VAR | sed -e 's/ *License//g'`
  fi fi
  test ".$VAR" = "." && k="(fallback)"
  ifelse($2,,[dnl here the defaults for LICENSE / COPYRIGHT
  if test ".$VAR"   = "."  ; then
    for ac_file in "$srcdir/COPYING" "$srcdir/COPYING" "$srcdir/LICENSE" ; do
      test -f "$ac_file" || continue
dnl  http://www.ibiblio.org/osrt/omf/omf_elements "16. Rights"
      if grep "GNU LESSER GENERAL PUBLIC LICENSE" "$ac_file" >/dev/null
      then VAR="GNU LGPL" ; break
      elif grep "GNU GENERAL PUBLIC LICENSE" "$ac_file" >/dev/null
      then VAR="GNU GPL" ; break
      elif grep "MOZILLA PUBLIC LICENSE" "$ac_file" >/dev/null
      then VAR="MPL" ; break
      elif grep "Mozilla Public License" "$ac_file" >/dev/null
      then VAR="MPL" ; break
      elif grep -i "artistic license" "$ac_file" >/dev/null
      then VAR="Artistic" ; break
      elif grep -i "artistic control" "$ac_file" >/dev/null
      then VAR="Artistic" ; break
      elif grep -i "semblance of artistic" "$ac_file" >/dev/null
      then VAR="Artistic" ; break
      elif grep -i "above copyright notice" "$ac_file" >/dev/null
      then VAR="BSD" ; break
      fi
    done
    if test ".$VAR" = "." ; then
      if test "$srcdir/COPYING.LIB" ; then VAR="GNU LGPL"
      elif test ".$ltmain" != "."   ; then VAR="GNU LGPL"
       else VAR="GNU GPL"
      fi
    fi
  fi
  ],[test ".$VAR" = "." && VAR="$2"])
  test "$VAR" = "GPL" && VAR="GNU GPL"
  test "$VAR" = "LGPL" && VAR="GNU LGPL"
  AC_MSG_RESULT([m4_ifval([$1],[$1 = ])$VAR $k])
  AS_VAR_POPDEF([VAR])dnl
])

AC_DEFUN([AX_SPEC_PACKAGE_SUMMARY],[AC_REQUIRE([AX_SPEC_FILE])dnl
  AS_VAR_PUSHDEF([VAR],[PACKAGE_SUMMARY])dnl
  AC_MSG_CHECKING([for spec summary])
  if test ".$VAR" = "." ; then if test ! -f $ax_spec_file
  then k="(w/o spec)"
  else k=""
    AX_SPEC_EXTRACT(VAR,[summary],m4_ifval($1,$1))
  fi fi
  test ".$VAR" = "." && k="(fallback)"
  ifelse($2,,[dnl here the defaults for SUMMARY
  if test ".$VAR"   = "."  ; then VAR="$PACKAGE"
     test ".$VAR" = "." && VAR="foo"
     test ".$ltmain" != "." && VAR="$VAR library"
  fi
  ],[test ".$VAR" = "." && VAR="$2"])
  AC_MSG_RESULT([m4_ifval([$1],[$1 = ])$VAR $k])
  AS_VAR_POPDEF([VAR])dnl
])

AC_DEFUN([AX_SPEC_PACKAGE_ICON],[AC_REQUIRE([AX_SPEC_FILE])dnl
  AS_VAR_PUSHDEF([VAR],[PACKAGE_ICON])dnl
  AC_MSG_CHECKING([for spec icon])
  if test ".$VAR" = "." ; then if test ! -f $ax_spec_file
  then k="(w/o spec)"
  else k=""
    AX_SPEC_EXTRACT(VAR,[icon],m4_ifval($1,$1))
  fi fi
  test ".$VAR" = "." && k="(fallback)"
  ifelse($2,,[dnl here the defaults for ICON
  if test ".$VAR"   = "."  ; then VAR="$PACKAGE-icon.png" ; fi
  ],[test ".$VAR" = "." && VAR="$2"])
  AC_MSG_RESULT([m4_ifval([$1],[$1 = ])$VAR $k])
  AS_VAR_POPDEF([VAR])dnl
])

AC_DEFUN([AX_SPEC_PACKAGE_CATEGORY],[AC_REQUIRE([AX_SPEC_FILE])dnl
  AS_VAR_PUSHDEF([VAR],[PACKAGE_CATEGORY])dnl
  AC_MSG_CHECKING([for spec category])
  if test ".$VAR" = "." ; then if test ! -f $ax_spec_file
  then k="(w/o spec)"
  else k=""
    AX_SPEC_EXTRACT(VAR,[group],m4_ifval($1,$1))
    VAR=`echo $VAR | sed -e 's/ /-/g'`
  fi fi
  test ".$VAR" = "." && k="(fallback)"
  ifelse($2,,[dnl here the defaults for CATEGORY
  if test ".$VAR" = "."  ; then if test ".$ltmain" != "."
     then VAR="Development/Library"
     else VAR="Development/Other"
  fi fi
  ],[test ".$VAR" = "." && VAR="$2"])
  AC_MSG_RESULT([m4_ifval([$1],[$1 = ])$VAR $k])
  AS_VAR_POPDEF([VAR])dnl
])

AC_DEFUN([AX_SPEC_PACKAGE_NAME],[AC_REQUIRE([AX_SPEC_FILE])dnl
  AS_VAR_PUSHDEF([VAR],[PACKAGE_NAME])dnl
  AC_MSG_CHECKING([for spec package])
  if test ".$VAR" = "." ; then if test ! -f $ax_spec_file
  then k="(w/o spec)"
  else k=""
    AX_SPEC_EXTRACT(VAR,[name],m4_ifval($1,$1))
    VAR=`echo $VAR | sed -e 's/ /-/g'`
  fi fi
  test ".$VAR" = "." && k="(fallback)"
  ifelse($2,,[dnl here the defaults for PACKAGE
  test ".$VAR"   = "."  && VAR=`basename $ax_spec_file .spec`
  test ".$VAR"   = ".README" && VAR="TODO"
  test ".$VAR"   = ".TODO" && VAR="foo"
  ],[test ".$VAR" = "." && VAR="$2"])
  test "VAR" = "PACKAGE_NAME" && test ".$PACKAGE" = "." && PACKAGE="$VAR"
  AC_MSG_RESULT([m4_ifval([$1],[$1 = ])$VAR $k])
  AS_VAR_POPDEF([VAR])dnl
])

AC_DEFUN([AX_SPEC_PACKAGE_VERSION_],[AC_REQUIRE([AX_SPEC_FILE])dnl
  AS_VAR_PUSHDEF([VAR],[PACKAGE_VERSION])dnl
  AC_MSG_CHECKING([for spec version])
  if test ".$VAR" = "." ; then if test ! -f $ax_spec_file
  then k="(w/o spec)"
  else k=""
    AX_SPEC_EXTRACT(VAR,[version],m4_ifval($1,$1))
    VAR=`echo $VAR | sed -e 's/ /-/g'`
  fi fi
  test ".$VAR" = "." && k="(fallback)"
  ifelse($2,,[dnl here the defaults for VERSION
  test ".$VAR"   = "."  && VAR=`date +0.%y.%W%w`
  ],[test ".$VAR" = "." && VAR="$2"])
  test "VAR" = "PACKAGE_VERSION" && test ".$VERSION" = "." && VERSION="$VAR"
  case "$VAR" in  # note we set traditional VERSION before cleaning things up
  *.*.) VAR="$VAR"`date +%W%w` ;;
  *.*.*) ;;
  *.)  VAR="$VAR"`date +%y.%W%w` ;;
  *.*) VAR="$VAR.0" ;;
  *) VAR=AS_TR_SH([$VAR]) ; VAR="$VAR.`date +%y.%W%w`" ;;
  esac
  VAR=`echo $VAR | sed -e "s/[[.]][0]\\([0-9]\\)/.\\1/g"`
  AC_MSG_RESULT([m4_ifval([$1],[$1 = ])$VAR $k])
  AS_VAR_POPDEF([VAR])dnl
])

dnl for compatibility, we define ax_spec_package_version
dnl to do all of ax_spec_package_name as well.
AC_DEFUN([AX_SPEC_PACKAGE_VERSION],[AC_REQUIRE([AX_SPEC_FILE])dnl
  ifelse($1,,
    AC_MSG_WARN([please use ax_spec_package_AND_version now!]),
    AC_MSG_ERROR([please use ax_spec_package_AND_version now!]))
  AX_SPEC_PACKAGE_NAME
  AX_SPEC_PACKAGE_VERSION_

])

AC_DEFUN([AX_SPEC_PACKAGE_AND_VERSION],[
  m4_ifset([m4_ax_spec_file],,[AX_SPEC_FILE($1)])
  AX_SPEC_PACKAGE_NAME
  AX_SPEC_PACKAGE_VERSION_
])

AC_DEFUN([AX_SPEC_DEFAULTS],[
  m4_ifset([m4_ax_spec_file],,[AX_SPEC_FILE($1)])
  AX_SPEC_PACKAGE_NAME
  AX_SPEC_PACKAGE_VERSION_
  AX_SPEC_PACKAGE_LICENSE
  AX_SPEC_PACKAGE_SUMMARY
  AX_SPEC_PACKAGE_CATEGORY
  AX_SPEC_PACKAGE_ICON
])

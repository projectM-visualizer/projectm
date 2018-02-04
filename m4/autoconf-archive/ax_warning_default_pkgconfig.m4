# =================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_warning_default_pkgconfig.html
# =================================================================================
#
# SYNOPSIS
#
#   AX_WARNING_DEFAULT_PKGCONFIGDIR [(dirvariable [,[defsetting][,[A][,[N/A]]]])]
#   AX_ENABLE_DEFAULT_PKGCONFIGDIR [(dirvariable [,defsetting])]
#
# DESCRIPTION
#
#   print a warning message if the $(datadir)/aclocal directory is not in
#   the dirlist searched by the aclocal tool. This macro is useful if some
#   `make install` would target $(datadir)/aclocal to install an autoconf m4
#   file of your project to be picked up by other projects.
#
#    default $1 dirvariable = pkgconfigdir
#    default $2 defsetting  = ${libdir}/pkgconfig
#    default $3 action = nothing to do
#    default $4 action = warn the user about mismatch
#
#   In the _WARNING_ variant, the defsetting is not placed in dirvariable
#   nor is it ac_subst'ed in any way. The default fail-action $4 is to send
#   a warning message to the user, and the default accept-action $3 is
#   nothing. It is expected that a Makefile is generated with
#   pkgconfigdir=${libdir}/pkgconfig
#
#   The _ENABLE_ variant however will set not only the $pkgconfigdir shell
#   var of the script, but it is also AC-SUBST'ed on default - and
#   furthermore a configure option "--enable-default-pkgconfigdir" is
#   provided. Only if that option is set then $2 default is not set to the
#   canonic default in the a $prefix subpath but instead $2 default is set
#   to the primary path where `pkg-config` looks for .pc files. The user may
#   also override the default on the command line.
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

#serial 8

AC_DEFUN([AX_WARNING_DEFAULT_PKGCONFIGDIR],[dnl
AC_REQUIRE([AX_EXPAND_PREFIX])dnl
AS_VAR_PUSHDEF([DIR],[ax_warning_default_pkgconfig_dir])dnl
AS_VAR_PUSHDEF([BIN],[ax_warning_default_pkgconfig_bin])dnl
AS_VAR_PUSHDEF([LOC],[ax_warning_default_pkgconfig_loc])dnl
LOC='m4_if([$2],,[${libdir}/pkgconfig],[$2])'
m4_ifval([$1],[test ".$[]$1" != "." && LOC="$[]$1"])
 if test ".$PKG_CONFIG" = "." ; then # we use the same default as in pkg.m4
    AC_PATH_PROG([PKG_CONFIG],[pkg-config],[no])
 fi
 if test "$PKG_CONFIG" = "no"
 then DIR="/" ; test -d "/usr/lib/pkgconfig" && DIR="/usr/lib/pkgconfig"
 else BIN=`AS_DIRNAME(["$DIR"])` ;
      if test -d "$BIN/lib/pkgconfig" ; then
          DIR="$BIN/lib/pkgconfig"
      else BIN=`AS_DIRNAME(["$DIR"])`
      if test -d "$BIN/lib/pkgconfig" ; then
          DIR="$BIN/lib/pkgconfig"
      else
      if test -d "/usr/lib/pkgconfig" ; then
          DIR="/usr/lib/pkgconfig"
      else
          DIR="/"
      fi fi fi
 fi
AC_RUN_LOG([: last pkgconfig dir is assumed as "$DIR"])
DIR=`eval "echo $DIR"`
DIR=`eval "echo $DIR"`
LOC=`eval "echo $LOC"`
LOC=`eval "echo $LOC"`
LOC=`eval "echo $LOC"`
LOC=`eval "echo $LOC"`
for DIR in `echo "$PKG_CONFIG_PATH:$DIR" | sed -e 's,:, ,g'` ; do
    AC_RUN_LOG([: test ".$LOC" = ".$DIR"])
    test ".$LOC" = ".$DIR" && break
done
if  test "$LOC" != "$DIR" ; then
        m4_ifval([$4],[$4],[dnl
 AC_MSG_NOTICE([warning: m4_if([$1],,[pkgconfigdir],[$1])=$LOC dnl
(see config.log)])
   AC_MSG_NOTICE([perhaps: make install m4_if([$1],,[pkgconfigdir],[$1])=$DIR])
   cat m4_ifset([AS_MESSAGE_LOG_FD],[>&AS_MESSAGE_LOG_FD],[>>config.log]) <<EOF
 pkgconfigdir:  the m4_if([$1],,[default pkgconfigdir],[$1 value]) of $LOC
 pkgconfigdir:  is not listed in the dirlist where pkg-config will look for
 pkgconfigdir:  package-configs - you can override the install-path using
 pkgconfigdir:  make install m4_if([$1],,[pkgconfigdir],[$1])=$DIR
 pkgconfigdir:  or set/append the directory to the environment variable
 pkgconfigdir:  PKG_CONFIG_PATH="$LOC"
EOF
   m4_ifvaln([$5],[$5])])dnl
   m4_ifvaln([$3],[else $3])dnl
fi
AS_VAR_POPDEF([LOC])dnl
AS_VAR_POPDEF([BIN])dnl
AS_VAR_POPDEF([DIR])dnl
])

AC_DEFUN([AX_ENABLE_DEFAULT_PKGCONFIGDIR],[dnl
AS_VAR_PUSHDEF([BIN],[ax_warning_default_pkgconfig_bin])dnl
AS_VAR_PUSHDEF([DIR],[ax_warning_default_pkgconfig_def])dnl
AS_VAR_PUSHDEF([DEF],[ax_warning_default_pkgconfig_def])dnl
AC_ARG_ENABLE([enable-default-pkgconfigdir],
[  --enable-default-pkgconfigdir(=PATH) override the libdir/pkgconfig default])
test ".$enable_default_pkgconfigdir" = "." && enable_default_pkgconfigdir="no"
case ".$enable_default_pkgconfigdir" in
  .no) DIR='m4_if([$2],,[${libdir}/pkgconfig],[$2])' ;;
  .yes) # autodetect
 if test ".$PKG_CONFIG" = "." ; then # we use the same default as in pkg.m4
    AC_PATH_PROG([PKG_CONFIG],[pkg-config],[no])
 fi
 if test "$PKG_CONFIG" = "no"
 then DIR="/tmp" ; test -d "/usr/lib/pkgconfig" && DIR="/usr/lib/pkgconfig"
 else BIN=`AS_DIRNAME(["$DIR"])` ;
      if test -d "$BIN/lib/pkgconfig" ; then
          DIR="$BIN/lib/pkgconfig"
      else BIN=`AS_DIRNAME(["$DIR"])`
      if test -d "$BIN/lib/pkgconfig" ; then
          DIR="$BIN/lib/pkgconfig"
      else
      if test -d "/usr/lib/pkgconfig" ; then
          DIR="/usr/lib/pkgconfig"
      else
          DIR="/tmp"
      fi fi fi
 fi ;;
  *) DIR="$enable_default_pkgconfigdir" ;;
esac
AX_WARNING_DEFAULT_PKGCONFIGDIR([$1],[$DEF],[$3],[$4],[$5])
m4_if([$1],,[pkgconfigdir],[$1])="$ax_warning_default_pkgconfig_dir"
AC_SUBST(m4_if([$1],,[pkgconfigdir],[$1]))
AS_VAR_POPDEF([DEF])dnl
AS_VAR_POPDEF([DIR])dnl
AS_VAR_POPDEF([BIN])dnl
])

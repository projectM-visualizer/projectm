# ================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_set_default_paths_system.html
# ================================================================================
#
# SYNOPSIS
#
#   AX_SET_DEFAULT_PATHS_SYSTEM
#
# DESCRIPTION
#
#   the most interesting changes go about windows-targets - where the
#   default_prefix is set to /programs, and quite some directories are
#   aliased: sbindir := libdir := bindir and the docprefix-defaults are also
#   a bit different, even on FHS2-compliant systems where the mandir is
#   going to $prefix/man only if prefix=/usr, otherwise they shall go to
#   $datadir/man. We use an extra docprefix to express it which is either
#   defined as being prefix or datadir. not SUBSTed here.
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

#serial 9

AU_ALIAS([AC_SET_DEFAULT_PATHS_SYSTEM], [AX_SET_DEFAULT_PATHS_SYSTEM])
AC_DEFUN([AX_SET_DEFAULT_PATHS_SYSTEM],
[AC_REQUIRE([AC_CANONICAL_HOST]) # --------------------------------------------
case "$prefix:$ac_default_prefix" in
  NONE:/usr/local)
    result=""
    AC_MSG_CHECKING(default prefix path)
    case "${target_os}" in
      *cygwin* | *mingw* | *uwin* | *djgpp | *emx* )
	if test "${host_os}" = "${target_os}" ; then
           ac_default_prefix="/programs"
           result="(win/dos target)"
        else
           case "$PATH" in
              *:/usr/local/cross-tools/$target_alias/bin:*)
		  ac_default_prefix="/usr/local/cross-tools/$target_alias" ;;
              *:/usr/local/$target_alias/bin:*)
		  ac_default_prefix="/usr/local/$target_alias" ;;
              *:/usr/local/$target_cpu-$target_os/bin:*)
		  ac_default_prefix="/usr/local/$target_cpu-$target_os" ;;
              *)
                  ac_default_prefix="/programs" ;;
           esac
           result="(win/dos cross-compiler)"
        fi
    ;;
    esac
    AC_MSG_RESULT($ac_default_prefix $result)
  ;;
esac
AC_MSG_CHECKING(default prefix system)
result="$prefix" ; test "$result" = "NONE" && result="$ac_default_prefix"
case ${result} in
  /programs | /programs/*) result="is win-/programs"
     # on win/dos, .exe .dll and .cfg live in the same directory
     libdir=`echo $libdir |sed -e 's:^..exec_prefix./lib$:${bindir}:'`
     sbindir=`echo $sbindir |sed -e 's:^..exec_prefix./sbin$:${libdir}:'`
     sysconfdir=`echo $sysconfdir |sed -e 's:^..prefix./etc$:${sbindir}:'`
     libexecdir=`echo $libexecdir |sed -e 's:/libexec$:/system:'`
     # help-files shall be set with --infodir, docprefix is datadir
     docprefix="${datadir}"
     mandir=`echo $mandir \
			     |sed -e 's:^..prefix./man$:${datadir}/info:'`
     includedir=`echo $includedir \
                |sed -e 's:^..prefix./include$:${datadir}/include:'`
     # other state files (but /etc) are moved to datadir
     sharedstatedir=`echo $sharedstatedir \
                     |sed -e 's:^..prefix./com$:${datadir}/default:'`
     localstatedir=`echo $localstatedir \
                     |sed -e 's:^..prefix./var$:${datadir}/current:'`
  ;;
  /usr) result="is /usr-shipped"
     # doc files are left at prefix
     docprefix="${prefix}"
     # state files go under /top
     sysconfdir=`echo $sysconfdir |sed -e 's:^..prefix./etc$:/etc:'`
     sharedstatedir=`echo $sharedstatedir \
                     |sed -e 's:^..prefix./com$:/etc/default:'`
     # $prefix/var is going to end up in /var/lib
     localstatedir=`echo $localstatedir \
                     |sed -e 's:^..prefix./var$:/var/lib:'`
  ;;
  /opt | /opt/*) result="is /opt-package"
     # state files go under /top/prefix
     sysconfdir=`echo $sysconfdir \
                     |sed -e 's:^..prefix./etc$:/etc${prefix}:'`
     sharedstatedir=`echo $sharedstatedir \
                     |sed -e 's:^..prefix./com$:/etc/default${prefix}:'`
     # $prefix/var is going to be /var$prefix... once again
     localstatedir=`echo $localstatedir \
                     |sed -e 's:^..prefix./var$:/var${prefix}:'`
     # doc files are left at prefix
     docprefix="${prefix}"
  ;;
  *) result="is /local-package"
     # doc files are moved from prefix down to datadir
     docprefix="${datadir}"
     mandir=`echo $mandir \
                     |sed -e 's:^..prefix./man$:${datadir}/man:'`
     infodir=`echo $infodir \
                     |sed -e 's:^..prefix./infodir$:${datadir}/info:'`
     # never use $prefix/com - that is no good idea
     sharedstatedir=`echo $sharedstatedir \
                     |sed -e 's:^..prefix./com$:${sysconfdir}/default:'`
  ;;
esac
AC_MSG_RESULT($result)
# --------------------------------------------------------
])

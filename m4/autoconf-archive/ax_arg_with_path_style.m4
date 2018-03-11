# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_arg_with_path_style.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_ARG_WITH_PATH_STYLE
#
# DESCRIPTION
#
#   _AC_DEFINE(PATH_STYLE) describing the filesys interface. The value is
#   numeric, where the basetype is encoded as 16 = dos/win, 32 = unix, 64 =
#   url/www, 0 = other
#
#   some extra semantics are described in other bits of the value,
#   especially
#
#    1024  accepts "/" as a dir separator
#    2048  accepts ";" as a path separator
#    4096  accepts "," as a path separator
#
#   the macro provides a configure' --with-path-style option that can be
#   used with descriptive arg names. If not explicitly given, the $target_os
#   will be checked to provide a sane default. Additional (lower) bits can
#   be used by the user for some additional magic, higher bits are reserved
#   for this macro.
#
#   the mnemonic "strict" or "also" is used to instruct the code that
#   additional separators shall be accepted but converted to the separator
#   of the underlying pathstyle system. (or-512)
#
#    example: --with-path-style=win,slash
#             to make it accept ";" as pathsep, and
#             both "/" and "\" as dirseps.
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

#serial 10

AU_ALIAS([AC_ARG_WITH_PATH_STYLE], [AX_ARG_WITH_PATH_STYLE])
AC_DEFUN([AX_ARG_WITH_PATH_STYLE],
[
 AC_ARG_WITH(path-style,
[  --with-path-style=[dos,unix,url,also,slash,comma],
[ac_with_path_style="$withval"],
[dnl
  case "$target_os" in
    *djgpp | *mingw32* | *emx*) ac_with_path_style="dos" ;;
    *) case `eval echo $exec_prefix` in
       *:*) ac_with_path_style="url" ;;
       *) ac_with_path_style="posix" ;;
       esac
    ;;
  esac
])
   ac_with_path_style__unx="0"
   ac_with_path_style__dos="0"
   ac_with_path_style__win="0"
   ac_with_path_style__mac="0"
   ac_with_path_style__def="0"
   ac_with_path_style__use="0"
   ac_with_path_style__slash="0"
   ac_with_path_style__semic="0"
   ac_with_path_style__comma="0"
   ac_with_path_style__level="0"

   case ",$ac_with_path_style," in
    *,unx,*|*,unix,*|*,bsd,*|*,posix,*) :
	ac_with_path_style__unx="32" ;;
    *)	ac_with_path_style__unx="0" ;;
   esac
   case ",$ac_with_path_style," in
    *,dos,*|*,win,*|*,windows,*) :
	ac_with_path_style__dos="16" ;;
    *)  ac_with_path_style__dos="0" ;;
   esac
   case ",$ac_with_path_style," in
    *,web,*|*,url,*|*,www,*) :
	ac_with_path_style__url="64" ;;
    *)	ac_with_path_style__url="0" ;;
   esac
   case ",$ac_with_path_style," in
    *,mac,*|*,macintosh,*|*,apple,*) :
	ac_with_path_style__mac="128" ;;
    *)	ac_with_path_style__mac="0" ;;
   esac
   case ",$ac_with_path_style," in
    *,def,*|*,define,*|*,special,*) :
	ac_with_path_style__def="256" ;;
    *)	ac_with_path_style__def="0" ;;
   esac
   case ",$ac_with_path_style," in
    *,also,*|*,strict,*|*,accept,*|*,convert,*) :
	ac_with_path_style__use="512" ;;
    *)	ac_with_path_style__use="0" ;;
   esac
   case ",$ac_with_path_style," in
    *,sl,*|*,slash,*|*,forwslash,*|*,slashsep,*) :
	ac_with_path_style__slash="1024" ;;
    *)	ac_with_path_style__slash="0" ;;
   esac
   case ",$ac_with_path_style," in
    *,sc,*|*,semi,*|*,semisep,*|*,semicolon,*|*,semicolonsep,*) :
	ac_with_path_style__semic="2048" ;;
    *)	ac_with_path_style__semic="0" ;;
   esac
   case ",$ac_with_path_style," in
    *,cm,*|*,comma,*|*,commasep,*) :
	ac_with_path_style__comma="4096" ;;
    *)	ac_with_path_style__comma="0" ;;
   esac

   if test "$ac_with_path_style__unx" != "0" ; then
	ac_with_path_style__slash="1024"
   fi
   if test "$ac_with_path_style__dos" != "0" ; then
	ac_with_path_style__semic="2048"
   fi
   if test "$ac_with_path_style__url" != "0" ; then
	ac_with_path_style__slash="1024"
	ac_with_path_style__semic="2048"
   fi

   case ",$ac_with_path_style," in
    *,7,*|*,all,*|*,muchmore,*)
	ac_with_path_style__level="7" ;;
    *,6,*|*,extra,*|*,manymore,*)
	ac_with_path_style__level="6" ;;
    *,5,*|*,much,*)
	ac_with_path_style__level="5" ;;
    *,4,*|*,many,*)
	ac_with_path_style__level="4" ;;
    *,3,*|*,plus,*|*,somemore,*)
	ac_with_path_style__level="3" ;;
    *,2,*|*,more,*)
	ac_with_path_style__level="2" ;;
    *,1,*|*,some,*)
	ac_with_path_style__level="1" ;;
    *)
	ac_with_path_style__level="0" ;;
   esac

   PATH_STYLE=`expr \
	$ac_with_path_style__unx '+' \
	$ac_with_path_style__dos '+' \
	$ac_with_path_style__win '+' \
	$ac_with_path_style__mac '+' \
	$ac_with_path_style__def '+' \
	$ac_with_path_style__use '+' \
	$ac_with_path_style__slash '+' \
	$ac_with_path_style__semic '+' \
	$ac_with_path_style__comma '+' \
        $ac_with_path_style__level `

 AC_DEFINE_UNQUOTED(PATH_STYLE,$PATH_STYLE,
 [ the OS pathstyle, 16=dos 32=unx 64=url 1024=slash 2048=semic 4096=comma ])
])

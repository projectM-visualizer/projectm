# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_subdir_files.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SUBDIR_FILES [(SUBDIRS [, CASEPATTERN])]
#
# DESCRIPTION
#
#   Look into subdirs and copy the (real) files that match pattern into the
#   local directory. Preferably we use a symbolic link of course. existing
#   local files are not overwritten.
#
#   The default casepattern is "*.?|*.cc|*.cpp", the default subdir-list
#   contains all subdirs available.
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

#serial 7

AU_ALIAS([AC_SUBDIR_FILES], [AX_SUBDIR_FILES])
AC_DEFUN([AX_SUBDIR_FILES],
[AC_BEFORE($0,[AX_PROG_CP_S])
  for ac_subdir in ifelse([$1], , *, $1) ; do
    if test -d $ac_subdir ; then
      AC_MSG_CHECKING(subdir $ac_subdir)
      for ac_file in $ac_subdir/* ; do
        if test -f $ac_file ; then
          if test ! -e `basename $ac_file` ; then
            case `basename $ac_file` in
              ifelse([$2], , *.?|*.cc|*.cpp,[$1]))
                echo ${ECHO_N} "$ac_file," ;
                $CP_S $ac_file . ;;
            esac
          fi
        fi
      done
      AC_MSG_RESULT(;)
    fi
  done
])

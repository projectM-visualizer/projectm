# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_cond_with_level.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_COND_WITH_LEVEL(PACKAGE [,DEFAULT [,YESLEVEL]])
#
# DESCRIPTION
#
#   Actually used after an AC_ARG_WITH(PKG,...) option-directive, where
#   AC_ARG_WITH is a part of the standard autoconf to define a `configure`
#   --with-PKG option.
#
#   This macros works almost like AC_COND_WITH(PACKAGE [,DEFAULT]), but the
#   definition is set to a numeric value. The level of a simple "yes" is
#   given (or assumed "2" if absent), otherwise the level can be given in
#   mnemonic names, being:
#
#    1 = some
#    2 = many
#    3 = all
#    4 = extra
#    5 = extrasome more
#    6 = extramany muchmore much
#    7 = super everything
#    8 = ultra experimental
#    9 = insane
#
#   this macro is most handily it making Makefile.in/Makefile.am that have a
#   set of with-level declarations, especially optimization-levels
#
#   the $withval shell-variable is set for further examination (it carries
#   the numeric value of the various mnemonics above)
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

#serial 11

AU_ALIAS([AC_COND_WITH_LEVEL], [AX_COND_WITH_LEVEL])
AC_DEFUN([AX_COND_WITH_LEVEL],
[dnl the names to be defined...
pushdef([WITH_VAR],    patsubst(with_$1, -, _))dnl
pushdef([VAR_WITH],    patsubst(translit(with_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHOUT], patsubst(translit(without_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHVAL], patsubst(translit(withval_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHDEF], patsubst(translit(withdef_$1, [a-z], [A-Z]), -, _))dnl
AC_SUBST(VAR_WITH)
AC_SUBST(VAR_WITHOUT)
AC_SUBST(VAR_WITHVAL)
AC_SUBST(VAR_WITHDEF)
if test -z "$WITH_VAR"
   then WITH_VAR=`echo ifelse([$2], , no, [$2])`
fi
if test "$WITH_VAR" = "yes"
   then WITH_VAR=`echo ifelse([$3], , some, [$3])`
fi
if test "$WITH_VAR" != "no"; then
  VAR_WITH=    ; VAR_WITHOUT='#'
  case "$WITH_VAR" in
    9|9,*|ultrasome|ultrasome,*|insane)				withval=9
		VAR_WITHVAL=9 ;  VAR_WITHDEF="-D""VAR_WITH=9" ;;
    8|8,*|ultra|ultra,*|experimental)				withval=8
                VAR_WITHVAL=8 ;  VAR_WITHDEF="-D""VAR_WITH=8" ;;
    7|7,*|muchmore|somemanymore|somemanymore,*|all)		withval=7
		VAR_WITHVAL=7 ;  VAR_WITHDEF="-D""VAR_WITH=7" ;;
    6|6,*|manymore|manymore,*|most)				withval=6
                VAR_WITHVAL=6 ;  VAR_WITHDEF="-D""VAR_WITH=6" ;;
    5|5,*|somemore|somemore,*|almost)				withval=5
		VAR_WITHVAL=5 ;  VAR_WITHDEF="-D""VAR_WITH=5" ;;
    4|4,*|more|more,*)						withval=4
		VAR_WITHVAL=4 ;  VAR_WITHDEF="-D""VAR_WITH=4" ;;
    3|3,*|much|somemany,*)					withval=3
		VAR_WITHVAL=3 ;  VAR_WITHDEF="-D""VAR_WITH=3" ;;
    2|2,*|many|many,*)						withval=2
		VAR_WITHVAL=2 ;  VAR_WITHDEF="-D""VAR_WITH=2" ;;
    1|1,*|some|some,*)						withval=1
		VAR_WITHVAL=1 ;  VAR_WITHDEF="-D""VAR_WITH=1" ;;
      *)							withval=-
	       AC_MSG_ERROR([dnl
 bad value of --with-$1 option, choose a number or some|many|more|all... ])
  esac
else								withval=0
  VAR_WITH='#' ;   VAR_WITHOUT=""
  VAR_WITHVAL=0 ;  VAR_WITHDEF=""
fi
popdef([VAR_WITH])dnl
popdef([VAR_WITHOUT])dnl
popdef([VAR_WITHVAL])dnl
popdef([VAR_WITHDEF])dnl
popdef([WITH_VAR])dnl
])

dnl and the same as AX_COND_WITH_LEVEL with an AC_DEFINE in the end...
AC_DEFUN([AX_COND_WITH_LEVEL_DEFINE],
[dnl the names to be defined...
pushdef([WITH_VAR],    patsubst(with_$1, -, _))dnl
pushdef([VAR_WITH],    patsubst(translit(with_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHOUT], patsubst(translit(without_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHVAL], patsubst(translit(withval_$1, [a-z], [A-Z]), -, _))dnl
pushdef([VAR_WITHDEF], patsubst(translit(withdef_$1, [a-z], [A-Z]), -, _))dnl
AC_SUBST(VAR_WITH)
AC_SUBST(VAR_WITHOUT)
AC_SUBST(VAR_WITHVAL)
AC_SUBST(VAR_WITHDEF)
if test -z "$WITH_VAR"
   then WITH_VAR=`echo ifelse([$2], , no, [$2])`
fi
if test "$WITH_VAR" = "yes"
   then WITH_VAR=`echo ifelse([$3], , some, [$3])`
fi
if test "$WITH_VAR" != "no"; then
  VAR_WITH=    ; VAR_WITHOUT='#'
  case "$WITH_VAR" in
    9|9,*|ultrasome|ultrasome,*|insane)				withval=9
		VAR_WITHVAL=9 ;  VAR_WITHDEF="-D""VAR_WITH=9" ;;
    8|8,*|ultra|ultra,*|experimental)				withval=8
                VAR_WITHVAL=8 ;  VAR_WITHDEF="-D""VAR_WITH=8" ;;
    7|7,*|muchmore|somemanymore|somemanymore,*|all)		withval=7
		VAR_WITHVAL=7 ;  VAR_WITHDEF="-D""VAR_WITH=7" ;;
    6|6,*|manymore|manymore,*|most)				withval=6
                VAR_WITHVAL=6 ;  VAR_WITHDEF="-D""VAR_WITH=6" ;;
    5|5,*|somemore|somemore,*|almost)				withval=5
		VAR_WITHVAL=5 ;  VAR_WITHDEF="-D""VAR_WITH=5" ;;
    4|4,*|more|more,*)						withval=4
		VAR_WITHVAL=4 ;  VAR_WITHDEF="-D""VAR_WITH=4" ;;
    3|3,*|much|somemany,*)					withval=3
		VAR_WITHVAL=3 ;  VAR_WITHDEF="-D""VAR_WITH=3" ;;
    2|2,*|many|many,*)						withval=2
		VAR_WITHVAL=2 ;  VAR_WITHDEF="-D""VAR_WITH=2" ;;
    1|1,*|some|some,*)						withval=1
		VAR_WITHVAL=1 ;  VAR_WITHDEF="-D""VAR_WITH=1" ;;
      *)							withval=-
	       AC_MSG_ERROR([dnl
 bad value of --with-$1 option, choose a number or some|many|more|all... ])
  esac
dnl -- the additional line is here --
  AC_DEFINE_UNQUOTED(VAR_WITH,$VAR_WITHVAL,"--with-$1=level")
else								withval=0
  VAR_WITH='#' ;   VAR_WITHOUT=""
  VAR_WITHVAL=0 ;  VAR_WITHDEF=""
fi
popdef([VAR_WITH])dnl
popdef([VAR_WITHOUT])dnl
popdef([VAR_WITHVAL])dnl
popdef([VAR_WITHDEF])dnl
popdef([WITH_VAR])dnl
])

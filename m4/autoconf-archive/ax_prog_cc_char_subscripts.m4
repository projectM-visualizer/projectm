# ===============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_prog_cc_char_subscripts.html
# ===============================================================================
#
# SYNOPSIS
#
#   AX_PROG_CC_CHAR_SUBSCRIPTS(substvar [,hard])
#
# DESCRIPTION
#
#   Try to find a compiler option that enables usage of char-type to index a
#   value-field. This one needs unsigned-chars and it must suppress warnings
#   about usage of chars for subscripting. for gcc -funsigned-char
#   -Wno-char-subscripts
#
#   Currently this macro knows about GCC. hopefully will evolve to use:
#   Solaris C compiler, Digital Unix C compiler, C for AIX Compiler, HP-UX C
#   compiler, and IRIX C compiler.
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

AU_ALIAS([AC_PROG_CC_CHAR_SUBSCRIPTS], [AX_PROG_CC_CHAR_SUBSCRIPTS])
AC_DEFUN([AX_PROG_CC_CHAR_SUBSCRIPTS], [
  pushdef([CV],ac_cv_prog_cc_char_subscripts)dnl
  hard=$2
  if test -z "$hard"; then
    msg="C to enable char subscripts"
  else
    msg="C to ensure char subscripts"
  fi
  AC_CACHE_CHECK($msg, CV, [
  cat > conftest.c <<EOF
int main (void)
{
   char v = 1;
   int x[[2]] = { 3 , 4 };
   return x[[v]];
}
EOF
  cp conftest.c writetest.c
  dnl GCC
  if test "$GCC" = "yes";
  then
	if test -z "$hard"; then
	   CV="-funsigned-char -Wno-char-subscripts"
	else
	   CV="-funsigned-char -Wno-char-subscripts"
	fi

	if test -n "`${CC-cc} -c $CV conftest.c 2>&1`" ; then
           CV="suppressed: did not work"
	fi

  dnl Solaris C compiler
	# Solaris sunpro has no option for unsignedchar but
	# signedchar is the default for char. Duhh.

  dnl HP-UX C compiler

  dnl Digital Unix C compiler
  elif ! $CC > /dev/null 2>&1 &&
	$CC -c -unsigned conftest.c > /dev/null 2>&1 &&
	test -f conftest.o
  then
	# char :  unsigned char
	CV="-unsigned"

	rm conftest.o
	if test -n "`${CC-cc} -c $CV conftest.c 2>&1`" ; then
           CV="suppressed: did not work"
	fi

  dnl C for AIX Compiler

  dnl IRIX C compiler
	# char is unsigned by default for IRIX C.

  fi
  rm -f conftest.*
  ])
  if test -z "[$]$1" ; then
    if test -n "$CV" ; then
      case "$CV" in
        suppressed*) $1="" ;; # known but suppressed
        *)  $1="$CV" ;;
      esac
    fi
  fi
  AC_SUBST($1)
  popdef([CV])dnl
])

# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_cf_ebcdic.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CF_EBCDIC
#
# DESCRIPTION
#
#   If the target character set is EBCDIC, defines variables cf_cv_ebcdic,
#   EBCDIC, and NOT_ASCII.
#
#   I originally wrote this and submitted it to the Lynx distribution.
#   Editorial revisions by Tom Dickey, et. al.
#
# LICENSE
#
#   Copyright (c) 2008 Paul Gilmartin <pg@sweng.stortek.com>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
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

#serial 6

dnl Check whether character set is EBCDIC.
AU_ALIAS([CF_EBCDIC], [AX_CF_EBCDIC])
AC_DEFUN([AX_CF_EBCDIC],[
AC_MSG_CHECKING(if character set is EBCDIC)
AC_CACHE_VAL(cf_cv_ebcdic,[
        AC_TRY_COMPILE([ ],
[ /* TryCompile function for CharSet.
   Treat any failure as ASCII for compatibility with existing art.
   Use compile-time rather than run-time tests for cross-compiler
   tolerance.  */
#if '0'!=240
make an error "Character set is not EBCDIC"
#endif ],
[ # TryCompile action if true
cf_cv_ebcdic=yes ],
[ # TryCompile action if false
cf_cv_ebcdic=no])
# end of TryCompile ])
# end of CacheVal CvEbcdic
AC_MSG_RESULT($cf_cv_ebcdic)
case "$cf_cv_ebcdic" in  #(vi
    yes) AC_DEFINE(EBCDIC)
         AC_DEFINE(NOT_ASCII);; #(vi
    *)   ;;
esac
])dnl

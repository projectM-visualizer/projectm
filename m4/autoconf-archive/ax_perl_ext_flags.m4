# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_perl_ext_flags.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PERL_EXT_FLAGS([CFLAGS-VARIABLE], [LDFLAGS-VARIABLE], [EXTRA-MODULES])
#   AX_PERL_EXT_CFLAGS([CFLAGS-VARIABLE])
#   AX_PERL_EXT_LDFLAGS([LDFLAGS-VARIABLE], [EXTRA-MODULES])
#
# DESCRIPTION
#
#   Fetches the linker flags and C compiler flags for compiling and linking
#   programs that embed a Perl interpreter. If the EXTRA-MODULES argument is
#   submitted, it is a space separated list of extra modules to link. The
#   flags will be stored in the provided variables.
#
#   Examples:
#
#     AX_PERL_EXT_FLAGS([PERLXS_CFLAGS], [PERLXS_LDFLAGS])
#     AC_SUBST([PERLXS_CFLAGS])
#     AC_SUBST([PERLXS_LDFLAGS])
#
#     AX_PERL_EXT_CFLAGS([PERLXS_CFLAGS])
#     AC_SUBST([PERLXS_CFLAGS])
#
#     AX_PERL_EXT_LDFLAGS([PERLXS_LDFLAGS], [-std Socket])
#
# LICENSE
#
#   Copyright (c) 2009 Mats Kindahl of Sun Microsystems <mats@sun.com>
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#   1. Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
#   3. The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
#   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
#   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
#   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
#   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#   POSSIBILITY OF SUCH DAMAGE.

#serial 8

AC_DEFUN([AX_PERL_EXT_CFLAGS],
[AC_CHECK_PROG(PERL,perl,perl)
 _AX_PERL_EXT_MODULE_CHECK([ExtUtils::Embed], [have_embed=yes],
                           [have_embed=no])
 AS_IF([test $have_embed = no],
       AC_MSG_ERROR([Require ExtUtils::Embed to proceed]))
 _AX_PERL_EXT_EMBED_CHECK([$1],[ccopts])
])


AC_DEFUN([AX_PERL_EXT_LDFLAGS],
[AC_CHECK_PROG(PERL,perl,perl)
 _AX_PERL_EXT_MODULE_CHECK([ExtUtils::Embed], [have_embed=yes],
                           [have_embed=no])
 AS_IF([test $have_embed = no],
       AC_MSG_ERROR([Require ExtUtils::Embed to proceed]))
 _AX_PERL_EXT_EMBED_CHECK([$1],[ldopts],[$2])
])


AC_DEFUN([AX_PERL_EXT_FLAGS],
[AC_CHECK_PROG(PERL,perl,perl)
 _AX_PERL_EXT_MODULE_CHECK([ExtUtils::Embed], [have_embed=yes],
                           [have_embed=no])
 AS_IF([test $have_embed = no],
       AC_MSG_ERROR([Require ExtUtils::Embed to proceed]))
 _AX_PERL_EXT_EMBED_CHECK([$1],[ccopts])
 _AX_PERL_EXT_EMBED_CHECK([$2],[ldopts],[$3])
])


dnl _AX_PERL_EXT_MODULE_CHECK(MODULE-NAME, ACTION-IF-FOUND, ACTION-IF-NOT-FOUND)
dnl
dnl Check for the existence of the perl module given by MODULE-NAME.
dnl
AC_DEFUN([_AX_PERL_EXT_MODULE_CHECK],
[AC_MSG_CHECKING([for perl module $1])
 $PERL "-M$1" -e exit > /dev/null 2>&1
 AS_IF([test $? -eq 0],
 [AC_MSG_RESULT(yes)
  $2],
 [AC_MSG_RESULT(no)
  $3])
])

dnl _AX_PERL_EXT_EMBED_CHECK(VARIABLE, COMMAND, [EXTRA-FLAGS]) Use
dnl
dnl ExtUtils::Embed fetch flags for embedding Perl in a C/C++
dnl application
dnl
AC_DEFUN([_AX_PERL_EXT_EMBED_CHECK],
[AC_MSG_CHECKING([for perl $2 embed flags])
 ax_c_perlxs_extras="$3"
 $1=`$PERL -MExtUtils::Embed -e $2 ${ax_c_perlxs_extras:+"-- $3"}`
 AC_MSG_RESULT($$1)
])

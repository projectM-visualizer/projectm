# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_c_declare_block.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_C_DECLARE_BLOCK
#
# DESCRIPTION
#
#   The macro will compile a test program to see whether the compiler does
#   allow new variable declarations in the middle of a C statement block,
#   i.e. after some non-declaration line. New compilers will allow that
#   which makes the behave a bit more like C++ - the gcc did support it for
#   quite a time already.
#
#   #define DECLARE_BLOCK_NEEDED says they need to be at the beginning of of
#   a statement block. Additionally two defines DECLARE_BLOCK { and
#   DECLARE_END } are being set. That makes it possible to do the following
#   in your source code (which this macro is really made up for):
#
#    #define ___ DECLARE_BLOCK
#    #define ____ DECLARE_END
#
#    int f() {
#     char buffer[1024];
#     fgets(buffer, 1024, stdin);
#     ___ int i; int ii = strlen(buffer);
#     for (i=0; i < ii; i++) {
#        fputc(buffer[i], stdout);
#     }____;
#    }
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

AC_DEFUN([AX_C_DECLARE_BLOCK],[dnl
AC_CACHE_CHECK(
 [if C variables must be declared at the beginning of a block],
 ax_cv_c_declare_block,[
 AC_TRY_COMPILE([#include <stdio.h>
 int f() {
   char buffer[1024];
   fgets(buffer, 1024, stdin);
   int i; int ii = strlen(buffer);
   for (i=0; i < ii; i++) {
     fputc(buffer[i], stdout);
   }
 }],
 [],
 ax_cv_c_declare_block=no, ax_cv_c_declare_block=yes)])
 if test "$ax_cv_c_declare_block" = yes; then
   AC_DEFINE([DECLARE_BLOCK_NEEDED],[1],
    [if C variables must be declared at the beginning of a block])
   AC_DEFINE([DECLARE_BLOCK],[{],
    [set to { if variable declarations need a block start before])
   AC_DEFINE([DECLARE_END],[}],
    [set to } if variable declarations need a block start before])
 fi
])

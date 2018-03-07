# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_c_compile_value.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_C_COMPILE_VALUE (COMPILE-VALUE, ALIAS, INCLUDES)
#
# DESCRIPTION
#
#   The AX_C_COMPILE_VALUE macro determines a compile time value by
#   generating the object code and reading the value from the code. Static
#   data initializers like sizeof(int) are unavailable to preprocessor. The
#   macro calculates the values known to compiler's static initializer.
#
#   Assumptions: The sought value should not exceed 65535. The shell
#   interpreter and the sed utility are expected to exist and work similarly
#   across possible build platforms.
#
#   Result: The resulting configure script will generate the preprocessor
#   symbol definition:
#
#     #define COMPILE_VALUE_<ALIAS> <NUMBER>
#
#   It was important that the value was embedded into the object file in a
#   predefined byte order during the test. This ensured that the result was
#   independent from the target platform's byte order.
#
#   The existing AC_CHECK_SIZEOF macro also computes the size of the given
#   type without running the test program. However, the existing macro will
#   produce a piece of configure script that will take the time proportional
#   to the logarithm of the sought value.
#
#   Example of use in configure.in:
#
#     AX_C_COMPILE_VALUE(sizeof(int), sizeof_int)
#     AX_C_COMPILE_VALUE([sizeof(int[[543]])], sizeof_int543)
#
#   As a result of running the generated configure script, the following
#   definition will appear in config.h:
#
#     #define COMPILE_VALUE_SIZEOF_INT 4
#     #define COMPILE_VALUE_SIZEOF_INT543 2172
#
# LICENSE
#
#   Copyright (c) 2008 Ilguiz Latypov
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
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

## Portability defines that help interoperate with classic and modern autoconfs
ifdef([AC_TR_SH],[
define([AC_TR_SH_REUSE],[AC_TR_SH([$1])])
define([AC_TR_CPP_REUSE],[AC_TR_CPP([$1])])
], [
define([AC_TR_SH_REUSE],
       [patsubst(translit([[$1]], [*+], [pp]), [[^a-zA-Z0-9_]], [_])])
define([AC_TR_CPP_REUSE],
       [patsubst(translit([[$1]],
                          [*abcdefghijklmnopqrstuvwxyz],
                          [PABCDEFGHIJKLMNOPQRSTUVWXYZ]),
                 [[^A-Z0-9_]], [_])])
])

AC_DEFUN([AX_C_COMPILE_VALUE], [
  pushdef([ac_c_compile_value],
    AC_TR_SH_REUSE([ac_cv_c_compile_value_$2]))dnl
  ac_c_compile_value_expand="$1"
  AC_CACHE_CHECK([value of $1 by analyzing object code],
                 ac_c_compile_value, [
    save_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS -c -o conftest.o"
    AC_TRY_COMPILE([$3
      #include <stddef.h>
      #include <stdint.h>
      #include <stdlib.h>
      #define COMPILE_VALUE $ac_c_compile_value_expand
      #define HEX_DIGIT(n)      ((n) >= 10 ? 'a' + (n) - 10 : '0' + (n))
      char object_code_block[] = {
        '\n', 'e', '4', 'V', 'A',
        '0', 'x',
        (char) HEX_DIGIT((((COMPILE_VALUE / 16) / 16) / 16) % 16),
        (char) HEX_DIGIT(((COMPILE_VALUE / 16) / 16) % 16),
        (char) HEX_DIGIT((COMPILE_VALUE / 16) % 16),
        (char) HEX_DIGIT(COMPILE_VALUE % 16),
        'Y', '3', 'p', 'M', '\n'
      };],
      [],
      [ac_c_compile_value=`
        typeset -i n=\`sed -ne 's/^e4VA0x\(.*\)Y3pM$/0x\1/p' < conftest.o\`;
        echo $n`],
      [ac_c_compile_value=0])
    CFLAGS="$save_CFLAGS"])
  AC_DEFINE_UNQUOTED(AC_TR_CPP_REUSE(compile_value_$2),
                     [$[]ac_c_compile_value],
                     [$1])
  popdef([ac_c_compile_value])dnl
])

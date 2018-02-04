# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_check_x86_features.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_X86_FEATURES([ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Checks if the host cpu supports various x86 instruction set, the
#   instructions that will get tested are "mmx, popcnt, sse, sse2, sse3,
#   sse4.1, sse4.2, sse4a, avx, avx2, avx512f, fma, fma4, bmi, bmi2". If the
#   instruction set is supported by the host cpu, the C preprocessor macro
#   HAVE_XXX_INSTRUCTIONS is set to 1. The XXX is up-cased instruction case
#   with dot replaced by underscore. For example, the test for "sse4.2"
#   would export HAVE_SSE4_2_INSTRUCTIONS=1. Also the compiler flag
#   "-msse4.2" would be added to X86_FEATURE_CFLAGS variable, that can be
#   obtained in Makefile.am using @X86_FEATURE_CFLAGS@.
#
#   If any of the test for the instruction set were succeeded, the configure
#   script would run ACTION-IF-FOUND if it is specified, or append
#   X86_FEATURE_CFLAGS to CFLAGS. If none of the instruction were found,
#   ACTION-IF-NOT-FOUND hook is triggered.
#
#   This macro requires gcc extended builtin function "__builtin_cpu_init"
#   and "__builtin_cpu_supports" to detect the cpu features. It will error
#   out if the compiler doesn't has these builtins.
#
#   See also AX_GCC_X86_CPU_SUPPORTS, which is the actual macro that perform
#   the checks for the instruction sets.
#
# LICENSE
#
#   Copyright (c) 2016 Felix Chern <idryman@gmail.com>
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

#serial 2

AC_DEFUN([AX_CHECK_X86_FEATURES],
 [m4_foreach_w(
   [ax_x86_feature],
   [mmx popcnt sse sse2 sse3 sse4.1 sse4.2 sse4a avx avx2 avx512f fma fma4 bmi bmi2],
   [AX_GCC_X86_CPU_SUPPORTS(ax_x86_feature,
     [X86_FEATURE_CFLAGS="$X86_FEATURE_CFLAGS -m[]ax_x86_feature"],
     [])
  ])
  AC_SUBST([X86_FEATURE_CFLAGS])
  m4_ifval([$1],[$1],
    [CFLAGS="$CFLAGS $X86_FEATURE_CFLAGS"])
  $2
])

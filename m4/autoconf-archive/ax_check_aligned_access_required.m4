# =====================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_check_aligned_access_required.html
# =====================================================================================
#
# SYNOPSIS
#
#   AX_CHECK_ALIGNED_ACCESS_REQUIRED
#
# DESCRIPTION
#
#   While the x86 CPUs allow access to memory objects to be unaligned it
#   happens that most of the modern designs require objects to be aligned -
#   or they will fail with a buserror. That mode is quite known by
#   big-endian machines (sparc, etc) however the alpha cpu is little-
#   endian.
#
#   The following function will test for aligned access to be required and
#   set a config.h define HAVE_ALIGNED_ACCESS_REQUIRED (name derived by
#   standard usage). Structures loaded from a file (or mmapped to memory)
#   should be accessed per-byte in that case to avoid segfault type errors.
#
#   The function checks if unaligned access would ignore the lowest bit of
#   the address. If that happens or if the test binary crashes, aligned
#   access is required.
#
#   If cross-compiling, assume that aligned access is needed to be safe. Set
#   ax_cv_have_aligned_access_required=no to override that assumption.
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

AC_DEFUN([AX_CHECK_ALIGNED_ACCESS_REQUIRED],
[AC_CACHE_CHECK([if pointers to integers require aligned access],
  [ax_cv_have_aligned_access_required],
  [AC_RUN_IFELSE([
    AC_LANG_PROGRAM([[@%:@include <stdlib.h>]],
                    [[
                      int i;
                      int *p;
                      int *q;
                      char *str;
                      str = (char *) malloc(40);
                      for (i = 0; i < 40; i++) {
                        *(str + i) = i;
                      }
                      p = (int *) (str + 1);
                      q = (int *) (str + 2);
                      return (*p == *q);
                    ]])],
     [ax_cv_have_aligned_access_required=no],
     [ax_cv_have_aligned_access_required=yes],
     [ax_cv_have_aligned_access_required=maybe])])

if test "x$ax_cv_have_aligned_access_required" = "xmaybe"; then
  AC_MSG_WARN([Assuming aligned access is required when cross-compiling])
  ax_cv_have_aligned_access_required=yes
fi

if test "x$ax_cv_have_aligned_access_required" = "xyes"; then
  AC_DEFINE([HAVE_ALIGNED_ACCESS_REQUIRED], [1],
    [Define if pointers to integers require aligned access])
fi
])

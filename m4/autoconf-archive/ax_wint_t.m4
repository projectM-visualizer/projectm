# ===========================================================================
#        https://www.gnu.org/software/autoconf-archive/ax_wint_t.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_WINT_T
#
# DESCRIPTION
#
#   It turns out that MacOS X doesn't guarantee to provide the "wint_t" type
#   unless you include the strange header <runetype.h> in addition to
#   <wchar.h>. Consequently, the "do I have a system wint_t?" probe fails
#   and an alternate gets used. Unfortunately, by the time the program gets
#   built, one way or another that funny header winds up getting included
#   and the "#define wint_t unsigned int" patch-up-hack mutilates the
#   typedef. This macro will get it right.
#
# LICENSE
#
#   Copyright (c) 2008 Bruce Korb <bkorb@gnu.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_WINT_T],[
 AC_CHECK_HEADERS([runetype.h wchar.h])
 AC_CHECK_TYPE(wint_t, [], [
   AC_DEFINE_UNQUOTED([wint_t], [unsigned int],
       [Define to `unsigned int' if not found])], [
   AC_INCLUDES_DEFAULT
   #if HAVE_RUNETYPE_H
   # include <runetype.h>
   #endif
   #if HAVE_WCHAR_H
   # include <wchar.h>
   #endif
   ])])

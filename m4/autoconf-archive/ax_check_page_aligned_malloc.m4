# =================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_check_page_aligned_malloc.html
# =================================================================================
#
# SYNOPSIS
#
#   AX_CHECK_PAGE_ALIGNED_MALLOC
#
# DESCRIPTION
#
#   Some operating systems (generally, BSD Unix variants) lack a
#   posix_memalign function, a memalign function, and a working (meaning,
#   the memory can be freed) valloc function. To make up for it, the malloc
#   function promises to return page-aligned addresses if more than one
#   page's worth of memory is allocated. AX_CHECK_PAGE_ALIGNED_MALLOC checks
#   for this condition and defines HAVE_PAGE_ALIGNED_MALLOC if the condition
#   holds.
#
#   As an aside, note that valloc'd memory cannot safely be freed on all
#   operating systems. (Again, some flavors of BSD are the troublemakers.)
#   It's best to avoid using valloc in favor of posix_memalign, memalign, or
#   an aligned malloc as detected by AX_CHECK_PAGE_ALIGNED_MALLOC.
#
#   Caveat: AX_CHECK_PAGE_ALIGNED_MALLOC takes a probabilistic approach. If
#   100 calls to malloc all return page-aligned addresses, it assumes that
#   all calls will behave likewise. It is therefore possible -- albeit
#   extremely unlikely -- that AX_CHECK_PAGE_ALIGNED_MALLOC can return a
#   false positive.
#
# LICENSE
#
#   Copyright (c) 2008 Scott Pakin <pakin@uiuc.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 10

AC_DEFUN([AX_CHECK_PAGE_ALIGNED_MALLOC],
[AC_CACHE_CHECK([if large mallocs guarantee page-alignment],
  [ax_cv_func_malloc_aligned],
  [AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#if HAVE_UNISTD_H
# include <unistd.h>
#endif

int main()
{
  int pagesize = getpagesize();
  int i;

  for (i=0; i<100; i++)
    if ((unsigned long)malloc(pagesize+1) & (pagesize-1))
      exit (1);
  exit (0);
}
              ],
     [ax_cv_func_malloc_aligned=yes],
     [ax_cv_func_malloc_aligned=no],
     [ax_cv_func_malloc_aligned=no])
  ])
if test "$ax_cv_func_malloc_aligned" = yes ; then
  AC_DEFINE([HAVE_PAGE_ALIGNED_MALLOC], [1],
    [Define if `malloc'ing more than one page always returns a page-aligned address.])
fi
])

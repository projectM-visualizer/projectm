# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_have_adns.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_HAVE_ADNS([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Check whether the GNU ADNS library available from
#   http://www.chiark.greenend.org.uk/~ian/adns/ is installed on this
#   system. The test is known to work with version 1.4 (or later). If the
#   library is found, the flags necessary to link it are added to $LIBS.
#
# LICENSE
#
#   Copyright (c) 2008 Peter Simons <simons@cryp.to>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AC_DEFUN([AX_HAVE_ADNS], [dnl
  AC_CHECK_HEADER([adns.h], [dnl
    AC_SEARCH_LIBS([adns_init], [adns], [dnl
      AC_CACHE_CHECK([for GNU ADNS], [ax_cv_have_adns], [dnl
	AC_LINK_IFELSE([dnl
	  AC_LANG_PROGRAM(
	    [#include <adns.h>],
	    [int rc; rc = adns_init((adns_state *)(0), adns_if_none, (FILE *)(0));])],
	  [ax_cv_have_adns=yes],
	  [ax_cv_have_adns=no])])])])
  AS_IF([test "${ax_cv_have_adns}" = "yes"], [$1], [$2])
])dnl

# ====================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_header_tr1_unordered_set.html
# ====================================================================================
#
# SYNOPSIS
#
#   AX_CXX_HEADER_TR1_UNORDERED_SET
#
# DESCRIPTION
#
#   Check whether the TR1 include <unordered_set> exists and define
#   HAVE_TR1_UNORDERED_SET if it does.
#
# LICENSE
#
#   Copyright (c) 2008 Benjamin Kosnik <bkoz@redhat.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([AC_CXX_HEADER_TR1_UNORDERED_SET], [AX_CXX_HEADER_TR1_UNORDERED_SET])
AC_DEFUN([AX_CXX_HEADER_TR1_UNORDERED_SET], [
  AC_CACHE_CHECK(for tr1/unordered_set,
  ax_cv_cxx_tr1_unordered_set,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <tr1/unordered_set>], [using std::tr1::unordered_set;],
  ax_cv_cxx_tr1_unordered_set=yes, ax_cv_cxx_tr1_unordered_set=no)
  AC_LANG_RESTORE
  ])
  if test "$ax_cv_cxx_tr1_unordered_set" = yes; then
    AC_DEFINE(HAVE_TR1_UNORDERED_SET,,[Define if tr1/unordered_set is present. ])
  fi
])

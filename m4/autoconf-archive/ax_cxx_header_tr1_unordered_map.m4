# ====================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_header_tr1_unordered_map.html
# ====================================================================================
#
# SYNOPSIS
#
#   AX_CXX_HEADER_TR1_UNORDERED_MAP
#
# DESCRIPTION
#
#   Check whether the TR1 include <unordered_map> exists and define
#   HAVE_TR1_UNORDERED_MAP if it does.
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

AU_ALIAS([AC_CXX_HEADER_TR1_UNORDERED_MAP], [AX_CXX_HEADER_TR1_UNORDERED_MAP])
AC_DEFUN([AX_CXX_HEADER_TR1_UNORDERED_MAP], [
  AC_CACHE_CHECK(for tr1/unordered_map,
  ax_cv_cxx_tr1_unordered_map,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <tr1/unordered_map>], [using std::tr1::unordered_map;],
  ax_cv_cxx_tr1_unordered_map=yes, ax_cv_cxx_tr1_unordered_map=no)
  AC_LANG_RESTORE
  ])
  if test "$ax_cv_cxx_tr1_unordered_map" = yes; then
    AC_DEFINE(HAVE_TR1_UNORDERED_MAP,,[Define if tr1/unordered_map is present. ])
  fi
])

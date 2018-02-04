# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_header_stdcxx_tr1.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_CXX_HEADER_STDCXX_TR1
#
# DESCRIPTION
#
#   Check for library coverage of the TR1 standard.
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

AU_ALIAS([AC_CXX_HEADER_STDCXX_TR1], [AX_CXX_HEADER_STDCXX_TR1])
AC_DEFUN([AX_CXX_HEADER_STDCXX_TR1], [
  AC_CACHE_CHECK(for ISO C++ TR1 include files,
  ax_cv_cxx_stdcxx_tr1,
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([
  #include <tr1/array>
  #include <tr1/ccomplex>
  #include <tr1/cctype>
  #include <tr1/cfenv>
  #include <tr1/cfloat>
  #include <tr1/cinttypes>
  #include <tr1/climits>
  #include <tr1/cmath>
  #include <tr1/complex>
  #include <tr1/cstdarg>
  #include <tr1/cstdbool>
  #include <tr1/cstdint>
  #include <tr1/cstdio>
  #include <tr1/cstdlib>
  #include <tr1/ctgmath>
  #include <tr1/ctime>
  #include <tr1/cwchar>
  #include <tr1/cwctype>
  #include <tr1/functional>
  #include <tr1/memory>
  #include <tr1/random>
  #include <tr1/regex>
  #include <tr1/tuple>
  #include <tr1/type_traits>
  #include <tr1/unordered_set>
  #include <tr1/unordered_map>
  #include <tr1/utility>
  ],,
  ax_cv_cxx_stdcxx_tr1=yes, ax_cv_cxx_stdcxx_tr1=no)
  AC_LANG_RESTORE
  ])
  if test "$ax_cv_cxx_stdcxx_tr1" = yes; then
    AC_DEFINE(STDCXX_TR1_HEADERS,,[Define if ISO C++ TR1 header files are present. ])
  fi
])

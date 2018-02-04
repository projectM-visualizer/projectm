# ============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_header_stdcxx_0x.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_CXX_HEADER_STDCXX_0X
#
# DESCRIPTION
#
#   Check for library coverage of the C++0x standard.
#
# LICENSE
#
#   Copyright (c) 2008 Benjamin Kosnik <bkoz@redhat.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AU_ALIAS([AC_CXX_HEADER_STDCXX_0X], [AX_CXX_HEADER_STDCXX_0X])
AC_DEFUN([AX_CXX_HEADER_STDCXX_0X], [
  AC_CACHE_CHECK(for ISO C++ 0x include files,
  ax_cv_cxx_stdcxx_0x,
  [AC_REQUIRE([AC_COMPILE_STDCXX_0X])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  ac_save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=gnu++0x"

  AC_TRY_COMPILE([
    #include <cassert>
    #include <ccomplex>
    #include <cctype>
    #include <cerrno>
    #include <cfenv>
    #include <cfloat>
    #include <cinttypes>
    #include <ciso646>
    #include <climits>
    #include <clocale>
    #include <cmath>
    #include <csetjmp>
    #include <csignal>
    #include <cstdalign>
    #include <cstdarg>
    #include <cstdbool>
    #include <cstddef>
    #include <cstdint>
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <ctgmath>
    #include <ctime>
    #include <cwchar>
    #include <cwctype>

    #include <algorithm>
    #include <array>
    #include <atomic>
    #include <bitset>
    #include <chrono>
    #include <complex>
    #include <condition_variable>
    #include <deque>
    #include <exception>
    #include <forward_list>
    #include <fstream>
    #include <functional>
    #include <future>
    #include <initializer_list>
    #include <iomanip>
    #include <ios>
    #include <iosfwd>
    #include <iostream>
    #include <istream>
    #include <iterator>
    #include <limits>
    #include <list>
    #include <locale>
    #include <map>
    #include <memory>
    #include <mutex>
    #include <new>
    #include <numeric>
    #include <ostream>
    #include <queue>
    #include <random>
    #include <ratio>
    #include <regex>
    #include <scoped_allocator>
    #include <set>
    #include <sstream>
    #include <stack>
    #include <stdexcept>
    #include <streambuf>
    #include <string>
    #include <system_error>
    #include <thread>
    #include <tuple>
    #include <typeindex>
    #include <typeinfo>
    #include <type_traits>
    #include <unordered_map>
    #include <unordered_set>
    #include <utility>
    #include <valarray>
    #include <vector>
  ],,
  ax_cv_cxx_stdcxx_0x=yes, ax_cv_cxx_stdcxx_0x=no)
  AC_LANG_RESTORE
  CXXFLAGS="$ac_save_CXXFLAGS"
  ])
  if test "$ax_cv_cxx_stdcxx_0x" = yes; then
    AC_DEFINE(STDCXX_0X_HEADERS,,[Define if ISO C++ 0x header files are present. ])
  fi
])

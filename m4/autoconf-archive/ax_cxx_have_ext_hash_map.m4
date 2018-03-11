# =============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_have_ext_hash_map.html
# =============================================================================
#
# SYNOPSIS
#
#   AX_CXX_HAVE_EXT_HASH_MAP
#
# DESCRIPTION
#
#   Check if the compiler has ext/hash_map Eg:
#
#     #if defined(HAVE_EXT_HASH_MAP)
#     #include <ext/hash_map>
#     #else
#     #if defined(HAVE_STL)
#     #include <hash_map>
#     #else
#     # Can't find hash_map header !
#     #endif
#     #endif
#
#   This file is Alain BARBET's AC_CXX_HAVE_EXT_HASH_SET 1.1 with
#   s/set/map/g :)
#
# LICENSE
#
#   Copyright (c) 2008 Perceval ANICHINI
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

#serial 6

AU_ALIAS([AC_CXX_HAVE_EXT_HASH_MAP], [AX_CXX_HAVE_EXT_HASH_MAP])
AC_DEFUN([AX_CXX_HAVE_EXT_HASH_MAP],
[AC_CACHE_CHECK(whether the compiler has ext/hash_map,
ax_cv_cxx_have_ext_hash_map,
[AC_REQUIRE([AX_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <ext/hash_map>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[hash_map<int, int> t; return 0;],
  ax_cv_cxx_have_ext_hash_map=yes, ax_cv_cxx_have_ext_hash_map=no)
  AC_LANG_RESTORE
])
if test "$ax_cv_cxx_have_ext_hash_map" = yes; then
   AC_DEFINE(HAVE_EXT_HASH_MAP,,[define if the compiler has ext/hash_map])
fi
])

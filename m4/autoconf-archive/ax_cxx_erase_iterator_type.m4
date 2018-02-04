# ===============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_cxx_erase_iterator_type.html
# ===============================================================================
#
# SYNOPSIS
#
#   AX_CXX_ERASE_ITERATOR_TYPE
#
# DESCRIPTION
#
#   If the compiler supports define erase with constant iterator or with
#   classical iterator.
#
#   Define autoconfigured_erase_iterator to const_iterator if supported and
#   iterator if not supported.
#
#   Define HAVE_ERASE_CONSTANT_ITERATOR if supported
#
# LICENSE
#
#   Copyright (c) 2015 Bastien ROUCARIES
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_CXX_ERASE_ITERATOR_TYPE],
[dnl
  AC_CACHE_CHECK([the type of iterator inside erase function],
                 ax_cv_cxx_erase_iterator_type,
  [dnl
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE([dnl
      AC_LANG_PROGRAM([
        #include <vector>

	typedef std::vector<int> intvector;

        struct myvector {
	  myvector() { v.push_back(1); }
	  typedef intvector::const_iterator const_iterator;
          typedef intvector::iterator iterator;
	  iterator erase (const_iterator position)
  	  { return v.erase(position); };
  	  iterator erase (const_iterator first, const_iterator last)
  	  { return v.erase(first,last); };
	  intvector v;
	};
	],
        [myvector v;
	 v.erase(v.v.begin(),v.v.end());
	 return 1;
	])],
      ax_cv_cxx_erase_iterator_type="const_iterator",
      ax_cv_cxx_erase_iterator_type="iterator")
    AC_LANG_POP([C++])
  ])
  AS_IF([test "X$ax_cv_cxx_erase_iterator_type" = Xconst_iterator],
    [AC_DEFINE(HAVE_ERASE_CONSTANT_ITERATOR,,[define if the compiler supports  iterator erase(const_iterator position)])])
  AH_VERBATIM([autoconfigured_erase_iterator],
  [/* Define to the keyword(s) used to specify the type of the
      erase iterator. Define to const_iterator if constant
      or iterator if not constant. */
  #undef autoconfigured_erase_iterator
  ])
  AC_DEFINE_UNQUOTED([autoconfigured_erase_iterator],
    [$ax_cv_cxx_erase_iterator_type],
    [iterator type for erase])
])

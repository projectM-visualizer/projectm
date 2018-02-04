# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_define_integer_bits.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_DEFINE_INTEGER_BITS (TYPE [, CANDIDATE-TYPE]...)
#
# DESCRIPTION
#
#   Given a TYPE of the form "int##_t" or "uint##_t", see if the datatype
#   TYPE is predefined. If not, then define TYPE -- both with AC_DEFINE and
#   as a shell variable -- to the first datatype of exactly ## bits in a
#   list of CANDIDATE-TYPEs. If none of the CANDIDATE-TYPEs contains exactly
#   ## bits, then set the TYPE shell variable to "no".
#
#   For example, the following ensures that uint64_t is defined as a 64-bit
#   datatype:
#
#     AX_DEFINE_INTEGER_BITS(uint64_t, unsigned long long, unsigned __int64, long)
#     if test "$uint64_t" = no; then
#       AC_MSG_ERROR([unable to continue without a 64-bit datatype])
#     fi
#
#   You should then put the following in your C code to ensure that all
#   datatypes defined by AX_DEFINE_INTEGER_BITS are visible to your program:
#
#     #include "config.h"
#
#     #if HAVE_INTTYPES_H
#     # include <inttypes.h>
#     #else
#     # if HAVE_STDINT_H
#     #  include <stdint.h>
#     # endif
#     #endif
#
# LICENSE
#
#   Copyright (c) 2008 Scott Pakin <pakin@uiuc.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([AC_DEFINE_INTEGER_BITS], [AX_DEFINE_INTEGER_BITS])
AC_DEFUN([AX_DEFINE_INTEGER_BITS],
[m4_define([ac_datatype_bits], [m4_translit($1, [a-zA-Z_])])
m4_define([ac_datatype_bytes], [m4_eval(ac_datatype_bits/8)])
AC_CHECK_TYPE($1, ,
 [
  AC_MSG_NOTICE([trying to find a suitable ]ac_datatype_bytes[-byte replacement for $1])
  $1=no
  find_$1 ()
  {
    _AX_DEFINE_INTEGER_BITS_HELPER($@)
    :
  }
  find_$1
  AC_DEFINE_UNQUOTED($1, $$1,
    [If not already defined, then define as a datatype of *exactly* ]ac_datatype_bits[ bits.])
 ])
])

dnl Iterate over arguments $2..$N, trying to find a good match for $1.
m4_define([_AX_DEFINE_INTEGER_BITS_HELPER],
[ifelse($2, , ,
 [m4_define([ac_datatype_bits], [m4_translit($1, [a-zA-Z_])])
  m4_define([ac_datatype_bytes], [m4_eval(ac_datatype_bits/8)])
  AC_CHECK_SIZEOF($2)
  if test "$AS_TR_SH(ac_cv_sizeof_$2)" -eq ac_datatype_bytes; then
    $1="$2"
    return
  fi
  _AX_DEFINE_INTEGER_BITS_HELPER($1, m4_shift(m4_shift($@)))
 ])
])

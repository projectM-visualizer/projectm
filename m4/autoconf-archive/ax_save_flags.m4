# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_save_flags.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SAVE_FLAGS([NAMESPACE])
#
# DESCRIPTION
#
#   Save common compilation flags into temporary variables.
#
#   Compilation flags includes: CPPFLAGS, CFLAGS, CXXFLAGS, LDFLAGS, LIBS,
#   OBJCFLAGS.
#
#   By default these flags are saved to a global (empty) namespace, but user
#   could specify a specific NAMESPACE to AX_SAVE_FLAGS macro and latter
#   restore it by using AX_RESTORE_FLAGS(NAMESPACE).
#
#     AX_SAVE_FLAGS(mypackage)
#     CPPFLAGS="-Imypackagespath ${CPPFLAGS}"
#     dnl .. do some detection ...
#     AX_RESTORE_FLAGS(mypackage)
#
# LICENSE
#
#   Copyright (c) 2009 Filippo Giunchedi <filippo@esaurito.net>
#   Copyright (c) 2011 The Board of Trustees of the Leland Stanford Junior University
#   Copyright (c) 2011 Russ Allbery <rra@stanford.edu>
#   Copyright (c) 2013 Bastien ROUCARIES <roucaries.bastien+autoconf@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

# list of flag to save
AC_DEFUN([_AX_SAVE_FLAGS_LIST],[dnl
[CCASFLAGS],dnl
[CFLAGS],dnl
[CPPFLAGS],dnl
[CXXFLAGS],dnl
[ERLCFLAGS],dnl
[FCFLAGS],dnl
[FCLIBS],dnl
[FFLAGS],dnl
[FLIBS],dnl
[GCJFLAGS],dnl
[JAVACFLAGS],dnl
[LDFLAGS],dnl
[LIBS],dnl
[OBJCFLAGS],dnl
[OBJCXXFLAGS],dnl
[UPCFLAGS],dnl
[VALAFLAGS]dnl
])

# save one flag in name space
AC_DEFUN([_AX_SAVE_ONE_FLAG],[
  AS_VAR_PUSHDEF([_ax_save_flag_var], [$2[]_$1[]_ax_save_flags])
  AS_VAR_COPY(_ax_save_flag_var, $2[])
  AS_VAR_POPDEF([_ax_save_flag_var])
])

AC_DEFUN([AX_SAVE_FLAGS],[dnl
   m4_foreach([FLAG], dnl
	      [_AX_SAVE_FLAGS_LIST()], dnl
	      [_AX_SAVE_ONE_FLAG([$1],FLAG)])
])

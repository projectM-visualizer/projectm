# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_restore_flags.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_RESTORE_FLAGS([namespace])
#
# DESCRIPTION
#
#   Restore common compilation flags from temporary variables.
#
#   Compilation flags includes: CPPFLAGS, CFLAGS, CXXFLAGS, LDFLAGS, LIBS,
#   OBJCFLAGS.
#
#   By default these flags are restored to a global (empty) namespace, but
#   user could restore from specific NAMESPACE by using
#   AX_RESTORE_FLAGS(NAMESPACE) macro.
#
#   Typical usage is like:
#
#     AX_SAVE_FLAGS(mypackage)
#     CPPFLAGS="-Imypackagespath ${CPPFLAGS}"
#     dnl ... do some detection ...
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

#serial 7

# save one flag in name space
AC_DEFUN([_AX_RESTORE_ONE_FLAG],[dnl
  AS_VAR_PUSHDEF([_ax_restore_flag_var], [$2[]_$1[]_ax_save_flags])
  AS_VAR_COPY($2[],_ax_restore_flag_var)
  AS_VAR_POPDEF([_ax_restore_flag_var])
])

AC_DEFUN([AX_RESTORE_FLAGS], [dnl
   m4_foreach([FLAG], dnl
	      [_AX_SAVE_FLAGS_LIST()], dnl
	      [_AX_RESTORE_ONE_FLAG([$1],FLAG)])
])

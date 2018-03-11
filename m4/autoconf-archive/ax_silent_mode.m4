# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_silent_mode.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SILENT_MODE(on|off)
#
# DESCRIPTION
#
#   Temporarily disable console output when running Autoconf macros. For
#   example:
#
#     AX_SILENT_MODE(on)    dnl disable console output
#     AC_PROG_CXX
#     AX_SILENT_MODE(off)   dnl enable console output
#     AC_PROG_RANLIB
#
# LICENSE
#
#   Copyright (c) 2008 Peter Simons <simons@cryp.to>
#   Copyright (c) 2008 Paolo Bonzini <bonzini@gnu.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_SILENT_MODE],
  [
  case "$1" in
    on)
      exec 6>/dev/null
      ;;
    off)
      exec 6>&1
      ;;
    *)
      AC_MSG_ERROR([Silent mode can only be switched "on" or "off".])
      ;;
  esac
  ])dnl

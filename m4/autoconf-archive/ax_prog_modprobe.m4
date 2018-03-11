# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_prog_modprobe.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_MODPROBE
#
# DESCRIPTION
#
#   This macro searches for a modprobe command, such as can be found on
#   Linux systems with loadable kernel module support. PATH is checked
#   first, then the default location(s).
#
#   This is one of several autoconf macros intended to assist in configuring
#   and installing loadable kernel modules.
#
# LICENSE
#
#   Copyright (c) 2008 Kaelin Colclasure <kaelin@acm.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([AC_PROG_MODPROBE], [AX_PROG_MODPROBE])
AC_DEFUN([AX_PROG_MODPROBE],[
AC_PATH_PROG(MODPROBE, modprobe, , $PATH:/sbin)
if test -z "$MODPROBE"; then
  AC_MSG_WARN([no support for loadable kernel modules])
else
  AC_MSG_CHECKING([for module_prefix])
  module_prefix=`$MODPROBE -c | grep path.kernel.= \
                 | sed -e '2,$d' -e 's/.*=//' -e 's/.kernel//'`
  AC_MSG_RESULT($module_prefix)
  AC_SUBST(module_prefix)
fi
])

# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_sysv_ipc.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SYSV_IPC
#
# DESCRIPTION
#
#   This macro checks for the SysV IPC header files. It only checks that you
#   can compile a program with them, not whether the system actually
#   implements working SysV IPC.
#
# LICENSE
#
#   Copyright (c) 2008 Warren Young <warren@etr-usa.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AU_ALIAS([ETR_SYSV_IPC], [AX_SYSV_IPC])
AC_DEFUN([AX_SYSV_IPC],
[
AC_CACHE_CHECK([for System V IPC headers], ac_cv_sysv_ipc, [
        AC_TRY_COMPILE(
                [
                        #include <sys/types.h>
                        #include <sys/ipc.h>
                        #include <sys/msg.h>
                        #include <sys/sem.h>
                        #include <sys/shm.h>
                ],, ac_cv_sysv_ipc=yes, ac_cv_sysv_ipc=no)
])

        if test x"$ac_cv_sysv_ipc" = "xyes"
        then
                AC_DEFINE(HAVE_SYSV_IPC, 1, [ Define if you have System V IPC ])
        fi
]) dnl AX_SYSV_IPC

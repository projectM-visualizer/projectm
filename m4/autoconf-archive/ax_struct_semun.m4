# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_struct_semun.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_STRUCT_SEMUN
#
# DESCRIPTION
#
#   This macro checks to see if sys/sem.h defines struct semun. Some systems
#   do, some systems don't. Your code must be able to deal with this
#   possibility; if HAVE_STRUCT_SEMUM isn't defined for a given system, you
#   have to define this structure before you can call functions like
#   semctl().
#
#   You should call AX_SYSV_IPC before this macro, to separate the check for
#   System V IPC headers from the check for struct semun.
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

AU_ALIAS([ETR_STRUCT_SEMUN], [AX_STRUCT_SEMUN])
AC_DEFUN([AX_STRUCT_SEMUN],
[
AC_CACHE_CHECK([for struct semun], ac_cv_struct_semun, [
        AC_TRY_COMPILE(
                [
                        #include <sys/types.h>
                        #include <sys/ipc.h>
                        #include <sys/sem.h>
                ],
                [ struct semun s; ],
                ac_cv_struct_semun=yes,
                ac_cv_struct_semun=no)
])

        if test x"$ac_cv_struct_semun" = "xyes"
        then
                AC_DEFINE(HAVE_STRUCT_SEMUN, 1,
                        [ Define if your system's sys/sem.h file defines struct semun ])
        fi
]) dnl AX_STRUCT_SEMUN

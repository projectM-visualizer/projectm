# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_short_sleep.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SHORT_SLEEP
#
# DESCRIPTION
#
#   This macro searches for a "sleep" function that has 1/1000 of a second
#   accuracy. On some systems, this is known as nap() and on others usleep()
#   / 1000. There are probably other functions like this defined in other
#   system libraries, but we don't know how to search for them yet.
#   Contributions joyously accepted. :)
#
# LICENSE
#
#   Copyright (c) 2008 Warren Young <warren@etr-usa.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 7

AU_ALIAS([ETR_SHORT_SLEEP], [AX_SHORT_SLEEP])
AC_DEFUN([AX_SHORT_SLEEP],
[
        AC_MSG_CHECKING([for nap() in libc])
        AC_TRY_LINK([ extern "C" long nap(long ms); ], [ nap(42); ],
                [
                        ax_ss_found=yes
                        ax_ss_factor=1
                        AC_DEFINE(HAVE_NAP,1,
                                [Define to use the nap() system call for short sleeps])
                        AC_MSG_RESULT(yes)
                ],
                [
                        AC_MSG_RESULT(no)
                        ax_ss_found=no
                ])

        if test x"$ax_ss_found" = "xno"
        then
                AC_MSG_CHECKING([for usleep()])
                AC_TRY_LINK([ #include <unistd.h> ], [ usleep(42); ],
                        [
                                ax_ss_found=yes
                                ax_ss_factor=1000
                                AC_DEFINE(HAVE_USLEEP,1,
                                        [Define to use the usleep() system call for short sleeps])
                                AC_MSG_RESULT(yes)
                        ],
                        [
                                AC_MSG_RESULT(no)
                                ax_ss_found=no
                        ])
        fi

        if test x"$ax_ss_found" = "xno"
        then
                save_LIBS=$LIBS
                LIBS="$LIBS -lx"
                AC_MSG_CHECKING([for nap() in libx])
                AC_TRY_LINK([ extern "C" long nap(long ms); ], [ nap(42); ],
                        [
                                ax_ss_found=yes
                                ax_ss_factor=1
                                AC_DEFINE(HAVE_NAP,1,
                                        [Define to use the nap() system call for short sleeps])
                                AC_MSG_RESULT(yes)
                        ],
                        [
                                AC_MSG_RESULT(no)
                                ax_ss_found=no
                        ])

                LIBS=$save_LIBS
                AX_SS_LIB=-lx
                AC_SUBST(AX_SS_LIB)
        fi

        if test x"$ax_ss_found" = "xyes"
        then
                AC_DEFINE_UNQUOTED(SHORT_SLEEP_FACTOR, $ax_ss_factor,
                        [Multiply milliseconds by this to get the argument for the short sleep system call])
        else
                AC_MSG_ERROR([Could not find a "short sleep" system call.])
        fi
])dnl AX_SHORT_SLEEP

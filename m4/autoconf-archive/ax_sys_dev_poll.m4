# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_sys_dev_poll.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SYS_DEV_POLL([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro tests for the presence of /dev/poll support in the build
#   environment. It checks that the needed structure (dvpoll) is available,
#   with the standard fields. /dev/poll is most often seen under Solaris.
#
#   Note that it does not attempt to actually open /dev/poll -- you should
#   test for errors when you open it and then fall back to poll() if it is
#   unavailable.
#
# LICENSE
#
#   Copyright (c) 2008 Dave Benson <daveb@ffem.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([AC_SYS_DEV_POLL], [AX_SYS_DEV_POLL])
AC_DEFUN([AX_SYS_DEV_POLL], [AC_CACHE_CHECK(for /dev/poll support, ac_cv_dev_poll,
    AC_TRY_COMPILE([#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/devpoll.h>],
[
  struct dvpoll p;
  p.dp_timeout = 0;
  p.dp_nfds = 0;
  p.dp_fds = (struct pollfd *) 0;
  return 0;
],
    ac_cv_dev_poll=yes
    [$1],
    ac_cv_dev_poll=no
    [$2]
    )
  )
])

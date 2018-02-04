# ============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_func_accept_argtypes.html
# ============================================================================
#
# SYNOPSIS
#
#   AX_FUNC_ACCEPT_ARGTYPES
#
# DESCRIPTION
#
#   Checks the data types of the three arguments to accept(). Results are
#   placed into the symbols ACCEPT_TYPE_ARG[123], consistent with the
#   following example:
#
#     #define ACCEPT_TYPE_ARG1 int
#     #define ACCEPT_TYPE_ARG2 struct sockaddr *
#     #define ACCEPT_TYPE_ARG3 socklen_t *
#
#   This macro requires AC_CHECK_HEADERS to have already verified the
#   presence or absence of sys/types.h and sys/socket.h.
#
#   NOTE: This is just a modified version of the AC_FUNC_SELECT_ARGTYPES
#   macro. Credit for that one goes to David MacKenzie et. al.
#
# LICENSE
#
#   Copyright (c) 2008 Daniel Richard G. <skunk@iskunk.org>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 6

AU_ALIAS([AC_FUNC_ACCEPT_ARGTYPES], [AX_FUNC_ACCEPT_ARGTYPES])
AC_DEFUN([AX_FUNC_ACCEPT_ARGTYPES],
[AC_MSG_CHECKING([types of arguments for accept()])
 AC_CACHE_VAL(ac_cv_func_accept_arg1,dnl
 [AC_CACHE_VAL(ac_cv_func_accept_arg2,dnl
  [AC_CACHE_VAL(ac_cv_func_accept_arg3,dnl
   [for ac_cv_func_accept_arg1 in 'int' 'unsigned int'; do
     for ac_cv_func_accept_arg2 in 'struct sockaddr *' 'void *'; do
      for ac_cv_func_accept_arg3 in 'socklen_t *' 'size_t *' 'unsigned int *' 'int *'; do
       AC_TRY_COMPILE(dnl
[#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
extern accept ($ac_cv_func_accept_arg1, $ac_cv_func_accept_arg2, $ac_cv_func_accept_arg3);],,dnl
        [ac_not_found=no ; break 3], ac_not_found=yes)
      done
     done
    done
   ])dnl AC_CACHE_VAL
  ])dnl AC_CACHE_VAL
 ])dnl AC_CACHE_VAL
 if test "$ac_not_found" = yes; then
  ac_cv_func_accept_arg1=int
  ac_cv_func_accept_arg2='struct sockaddr *'
  ac_cv_func_accept_arg3='socklen_t *'
 fi
 AC_MSG_RESULT([$ac_cv_func_accept_arg1, $ac_cv_func_accept_arg2, $ac_cv_func_accept_arg3])
 AC_DEFINE_UNQUOTED(ACCEPT_TYPE_ARG1,$ac_cv_func_accept_arg1)
 AC_DEFINE_UNQUOTED(ACCEPT_TYPE_ARG2,$ac_cv_func_accept_arg2)
 AC_DEFINE_UNQUOTED(ACCEPT_TYPE_ARG3,$ac_cv_func_accept_arg3)
])

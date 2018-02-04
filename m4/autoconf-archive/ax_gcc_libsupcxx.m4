# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_gcc_libsupcxx.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_GCC_LIBSUPCXX(VARIABLE)
#
# DESCRIPTION
#
#   AX_GCC_LIBSUPCXX defines VARIABLE as the absolute path to libsupc++.a if
#   it is available on the system, empty otherwise.
#
# LICENSE
#
#   Copyright (c) 2009 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#   Copyright (c) 2009 Alessandro Massignan <ff0000.it@gmail.com>
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

#serial 8

AC_DEFUN([AX_GCC_LIBSUPCXX], [
    AX_GCC_LIBRARIES_DIR([GCC_LIBRARIES_DIR])
    AS_IF([test -z "$GCC_LIBRARIES_DIR"],[
        AC_MSG_ERROR([problems detecting gcc libraries dir])
    ])

    AC_MSG_CHECKING([for libsupc++.a])

    ax_gcc_libraries_dir_IFS=$IFS
    IFS=":"

    $1=""
    for i in $GCC_LIBRARIES_DIR
    do
        AS_IF([test -f "$i/libsupc++.a"],[
             IFS=$ax_gcc_libraries_dir_IFS
             $1="$i/libsupc++.a"
             break
        ])
    done

    IFS=$ax_gcc_libraries_dir_IFS

    AC_MSG_RESULT([$$1])
])

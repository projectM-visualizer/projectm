# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_prog_xsltproc.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_XSLTPROC([default-flags])
#
# DESCRIPTION
#
#   Find an xsltproc executable.
#
#   Input:
#
#   "default-flags" is the default $XSLTPROC_FLAGS, which will be overridden
#   if the user specifies --with-xsltproc-flags.
#
#   Output:
#
#   $XSLTPROC contains the path to xsltproc, or is empty if none was found
#   or the user specified --without-xsltproc. $XSLTPROC_FLAGS contains the
#   flags to use with xsltproc.
#
# LICENSE
#
#   Copyright (c) 2008,2009 Zmanda Inc. <http://www.zmanda.com/>
#   Copyright (c) 2008,2009 Dustin J. Mitchell <dustin@zmanda.com>
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

#serial 9

AU_ALIAS([AC_PROG_XSLTPROC], [AX_PROG_XSLTPROC])
AC_DEFUN([AX_PROG_XSLTPROC],
[
XSLTPROC_FLAGS="$1"
AC_SUBST(XSLTPROC_FLAGS)

# The (lack of) whitespace and overquoting here are all necessary for
# proper formatting.
AC_ARG_WITH(xsltproc,
AS_HELP_STRING([--with-xsltproc[[[[[=PATH]]]]]],
               [Use the xsltproc binary in PATH.]),
    [ ac_with_xsltproc=$withval; ],
    [ ac_with_xsltproc=maybe; ])

AC_ARG_WITH(xsltproc-flags,
AS_HELP_STRING([  --with-xsltproc-flags=FLAGS],
               [Flags to pass to xsltproc (default $1)]),
    [ if test "x$withval" = "xno"; then
        XSLTPROC_FLAGS=''
    else
        if test "x$withval" != "xyes"; then
            XSLTPROC_FLAGS="$withval"
        fi
    fi
        ])

# search for xsltproc if it wasn't specified
if test "$ac_with_xsltproc" = "yes" -o "$ac_with_xsltproc" = "maybe"; then
    AC_PATH_PROGS(XSLTPROC,xsltproc)
else
    if test "$ac_with_xsltproc" != "no"; then
        if test -x "$ac_with_xsltproc"; then
            XSLTPROC="$ac_with_xsltproc";
        else
            AC_MSG_WARN([Specified xsltproc of $ac_with_xsltproc isn't])
            AC_MSG_WARN([executable; searching for an alternative.])
            AC_PATH_PROGS(XSLTPROC,xsltproc)
        fi
    fi
fi
])

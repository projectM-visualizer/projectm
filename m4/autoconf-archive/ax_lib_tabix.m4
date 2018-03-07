# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_lib_tabix.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_TABIX()
#
# DESCRIPTION
#
#   This macro searches for an installed tabix library. If nothing was
#   specified when calling configure, it searches first in /usr/local and
#   then tries with ld's default library search path. If the
#   --with-tabix=DIR is specified, it will try to find it in
#   DIR/include/tabix/tabix.h and DIR/lib/libtabix.a. As a final try it will
#   look in DIR/tabix.h and DIR/libtabix.a as the tabix library does not
#   contain an install rule.
#
#   If --without-tabix is specified, the library is not searched at all.
#
#   If either the header file (tabix.h) or the library (libtabix) is not
#   found, the configuration exits on error, asking for a valid tabix
#   installation directory or --without-tabix.
#
#   The macro defines the symbol HAVE_TABIX if the library is found. You
#   should use autoheader to include a definition for this symbol in a
#   config.h file. Sample usage in a C/C++ source is as follows:
#
#     #ifdef HAVE_TABIX
#     #include <tabix.h>
#     #endif /* HAVE_TABIX */
#
#   The following output variables are set with AC_SUBST:
#
#     TABIX_CPPFLAGS
#     TABIX_LDFLAGS
#     TABIX_LIBS
#
#   You can use them like this in Makefile.am:
#
#     AM_CPPFLAGS = $(TABIX_CPPFLAGS)
#     AM_LDFLAGS = $(TABIX_LDFLAGS)
#     program_LDADD = $(TABIX_LIBS)
#
# LICENSE
#
#   Copyright (c) 2013 Timothy Brown <tbrown@freeshell.org>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 3 of the License, or (at your
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

#serial 3

AC_DEFUN([AX_LIB_TABIX],
#
# Handle user hints
#
[AC_MSG_CHECKING([if tabix is wanted])
AC_ARG_WITH([tabix],
  AS_HELP_STRING([--with-tabix],
                 [search for tabix in DIR/include and DIR/lib]),
 [if test "$withval" != no ; then
   AC_MSG_RESULT([yes])
   if test -d "$withval" ; then
     TABIX_HOME="$withval"
   else
     AC_MSG_WARN([Sorry, $withval does not exist, checking usual places])
   fi
 else
   AC_MSG_RESULT([no])
 fi],
 [AC_MSG_RESULT([yes])])

if test -f "${TABIX_HOME}/include/tabix/tabix.h" ; then
        TABIX_INCDIR="-I${TABIX_HOME}/include/tabix"
        TABIX_LIBDIR="-L${TABIX_HOME}/lib"
elif test -f "${TABIX_HOME}/include/tabix.h" ; then
        TABIX_INCDIR="-I${TABIX_HOME}/include"
        TABIX_LIBDIR="-L${TABIX_HOME}/lib"
elif test -f "${TABIX_HOME}/tabix.h" ; then
        TABIX_INCDIR="-I${TABIX_HOME}"
        TABIX_LIBDIR="-L${TABIX_HOME}"
elif test -f "/usr/local/include/tabix/tabix.h" ; then
        TABIX_HOME="/usr/local"
        TABIX_INCDIR="-I${TABIX_HOME}/include/tabix"
        TABIX_LIBDIR="-L${TABIX_HOME}/lib"
else
        TABIX_HOME="/usr"
        TABIX_INCDIR="-I${TABIX_HOME}/include/tabix"
        TABIX_LIBDIR=""
fi

#
# Locate tabix, if wanted
#
if test -n "${TABIX_HOME}" ; then

        TABIX_OLD_LDFLAGS=$LDFLAGS
        TABIX_OLD_CPPFLAGS=$LDFLAGS
        LDFLAGS="$LDFLAGS ${TABIX_LIBDIR}"
        CPPFLAGS="$CPPFLAGS ${TABIX_INCDIR}"
        AC_LANG_SAVE
        AC_LANG_C
        AC_CHECK_HEADER([tabix.h], [ac_cv_tabix_h=yes], [ac_cv_tabix_h=no])
        AC_CHECK_LIB([tabix],[ti_open],[ac_cv_libtabix=yes],[ac_cv_libtabix=no])
        AC_LANG_RESTORE
        if test "$ac_cv_libtabix" = "yes" && \
           test "$ac_cv_tabix_h"  = "yes" ; then
                #
                # If both library and header were found, use them
                #
                AC_MSG_CHECKING([tabix])
                AC_MSG_RESULT([ok])
                with_tabix=yes
        else
                #
                # If either header or library was not found, revert and bomb
                #
                LDFLAGS="$TABIX_OLD_LDFLAGS"
                CPPFLAGS="$TABIX_OLD_CPPFLAGS"
                AC_MSG_CHECKING([tabix])
                AC_MSG_RESULT([failed])
                AC_MSG_ERROR([either specify a valid tabix installation with --with-tabix=DIR or disable tabix usage with --without-tabix])
        fi
fi
])

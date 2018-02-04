# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_lib_samtools.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_SAMTOOLS()
#
# DESCRIPTION
#
#   This macro searches for an installed samtools library. If nothing was
#   specified when calling configure, it searches first in /usr/local and
#   then tries with ld's default library search path. If the
#   --with-samtools=DIR is specified, it will try to find it in
#   DIR/include/bam/sam.h and DIR/lib/libbam.a. As a final try it will look
#   in DIR/sam.h and DIR/libbam.a as the samtools library does not contain
#   an install rule.
#
#   If --without-samtools is specified, the library is not searched at all.
#
#   If either the header file (sam.h) or the library (libbam) is not found,
#   the configuration exits on error, asking for a valid samtools
#   installation directory or --without-samtools.
#
#   The macro defines the symbol HAVE_SAMTOOLS if the library is found. You
#   should use autoheader to include a definition for this symbol in a
#   config.h file. Sample usage in a C/C++ source is as follows:
#
#     #ifdef HAVE_SAMTOOLS
#     #include <sam.h>
#     #endif /* HAVE_SAMTOOLS */
#
#   The following output variables are set with AC_SUBST:
#
#     SAMTOOLS_CPPFLAGS
#     SAMTOOLS_LDFLAGS
#     SAMTOOLS_LIBS
#
#   You can use them like this in Makefile.am:
#
#     AM_CPPFLAGS = $(SAMTOOLS_CPPFLAGS)
#     AM_LDFLAGS = $(SAMTOOLS_LDFLAGS)
#     program_LDADD = $(SAMTOOLS_LIBS)
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

AC_DEFUN([AX_LIB_SAMTOOLS],
#
# Handle user hints
#
[AC_MSG_CHECKING([if samtools is wanted])
AC_ARG_WITH([samtools],
  AS_HELP_STRING([--with-samtools],
                 [search for samtools in DIR/include and DIR/lib]),
 [if test "$withval" != no ; then
   AC_MSG_RESULT([yes])
   if test -d "$withval" ; then
     SAMTOOLS_HOME="$withval"
   else
     AC_MSG_WARN([Sorry, $withval does not exist, checking usual places])
   fi
 else
   AC_MSG_RESULT([no])
 fi],
 [AC_MSG_RESULT([yes])])

if test -f "${SAMTOOLS_HOME}/include/bam/sam.h" ; then
        SAMTOOLS_INCDIR="-I${SAMTOOLS_HOME}/include/bam"
        SAMTOOLS_LIBDIR="-L${SAMTOOLS_HOME}/lib"
elif test -f "${SAMTOOLS_HOME}/include/sam.h" ; then
        SAMTOOLS_INCDIR="-I${SAMTOOLS_HOME}/include"
        SAMTOOLS_LIBDIR="-L${SAMTOOLS_HOME}/lib"
elif test -f "${SAMTOOLS_HOME}/sam.h" ; then
        SAMTOOLS_INCDIR="-I${SAMTOOLS_HOME}"
        SAMTOOLS_LIBDIR="-L${SAMTOOLS_HOME}"
elif test -f "/usr/local/include/bam/sam.h" ; then
        SAMTOOLS_HOME="/usr/local"
        SAMTOOLS_INCDIR="-I${SAMTOOLS_HOME}/include/bam"
        SAMTOOLS_LIBDIR="-L${SAMTOOLS_HOME}/lib"
else
        SAMTOOLS_HOME="/usr"
        SAMTOOLS_INCDIR="-I${SAMTOOLS_HOME}/include/bam"
        SAMTOOLS_LIBDIR=""
fi

#
# Locate samtools, if wanted
#
if test -n "${SAMTOOLS_HOME}" ; then

        SAMTOOLS_OLD_LDFLAGS=$LDFLAGS
        SAMTOOLS_OLD_CPPFLAGS=$LDFLAGS
        LDFLAGS="$LDFLAGS ${SAMTOOLS_LIBDIR}"
        CPPFLAGS="$CPPFLAGS ${SAMTOOLS_INCDIR}"
        AC_LANG_SAVE
        AC_LANG_C
        AC_CHECK_HEADER([sam.h], [ac_cv_sam_h=yes], [ac_cv_sam_h=no])
        AC_CHECK_LIB([bam], [sam_open], [ac_cv_libbam=yes], [ac_cv_libbam=no])
        AC_LANG_RESTORE
        if test "$ac_cv_libbam" = "yes" && test "$ac_cv_sam_h" = "yes" ; then
                #
                # If both library and header were found, use them
                #
                AC_MSG_CHECKING([samtools])
                AC_MSG_RESULT([ok])
                with_samtools=yes
        else
                #
                # If either header or library was not found, revert and bomb
                #
                LDFLAGS="$SAMTOOLS_OLD_LDFLAGS"
                CPPFLAGS="$SAMTOOLS_OLD_CPPFLAGS"
                AC_MSG_CHECKING([samtools])
                AC_MSG_RESULT([failed])
                AC_MSG_ERROR([either specify a valid samtools installation with --with-samtools=DIR or disable samtools usage with --without-samtools])
        fi
fi
])

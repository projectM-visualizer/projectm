# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_pkg_mico.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PKG_MICO([X.X.X optional required])
#
# DESCRIPTION
#
#   This macro searches for a MICO installation on your system. MICO (Mico
#   Is COrba) installs the file 'mico-setup.sh' under MICO_INSTALL_BASE/lib,
#   where MICO_INSTALL_BASE is probably /usr/local or /usr. If nothing was
#   specified when calling configure or just --with-mico, it searches in
#   $prefix/lib:/usr/local/lib:/usr/lib The argument of s--with-mico
#   specifies the full pathname of the mico-setup.sh file. For instance
#   --with-mico=/usr/local/mico-2.2.6/lib/mico-setup.sh.
#
#   If the version argument is given (e.g. 2.2.6), AX_PKG_MICO checks that
#   the mico package is this version number or higher.
#
#   The other optional argument to AX_PKG_MICO are 'optional' and
#   'required'. by default, placing AX_PKG_MICO in your configure.in does
#   not enforce using mico, if it is not found on the system. If MICO
#   existence is critical for the building of your package, then you
#   probably want to specify something like "AX_PKG_MICO(2.2.6 required)".
#   Then, even if the --with-mico was not specified on the command line of
#   the configure script, an error will be raised if mico was not found.
#
#   If the mico package is not found, abort configuration with error
#   message.
#
#   It defines the symbol MICOSETUP, MICOVERSION, MICOSHAREDDIR
#   MICO_INCLUDE, MICO_LIB and MICO_CXX if mico is found. In addition, the
#   mico script mico-c++ (set in (MICO_CXX) is scaned for a string match
#   "-I.*ministl" and if foundMICO_USE_MINISTL is set to 1, otherwise to 0.
#
#   Example of use of these variables in your Makefile.in:
#
#     INCLUDES += @MICO_INCLUDE@
#     LDFLAGS += @MICO_LIB@
#
# LICENSE
#
#   Copyright (c) 2008 Tal Shalif <tal@shalif.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 9

AU_ALIAS([AC_PKG_MICO], [AX_PKG_MICO])
AC_DEFUN([AX_PKG_MICO],
[
#
# first parse the argument given in configure.in to AX_PKG_MICO
#
MICO_REQUEST_VERSION=
MICO_URGENCY="optional"

changequote(<<, >>)

for a in $1 $2 $3 $4 $5 $6 $7 $8 $9 x; do
    case "$a" in
        x) break;;
        [0-9]*.[0-9]*.[0-9]*) MICO_REQUEST_VERSION="$a";;
        optional|required) MICO_URGENCY="$a";;
        *) argerror="argument to the PKG_MICO macro must be one of 'X.X.X' 'required' or 'optional'";;
    esac
done

changequote([, ])

if test -n "$argerror"; then
  AC_MSG_ERROR($argerror)
fi;

AC_MSG_CHECKING(if mico is wanted)
AC_ARG_WITH(mico,
    [  --with-mico=PATH absolute path name of mico's configuration file 'mico-setup.sh' (default is to search in \$prefix/lib:/usr/local/lib:/usr/lib),
    --without-mico to disable mico detection],
    [
    #
    # Run this if -with or -without was specified
    #
    case "$withval" in
        yes) MICO_URGENCY=required;;
        no) if test x$MICO_URGENCY = xrequired; then
                AC_MSG_ERROR("PKG_MICO was configured with the 'required' option. You cannot override it from the command line")
            fi;
            ;;
        *) MICO_URGENCY=required; MICOSETUP="$withval" ;;
    esac

    ],[])

AC_MSG_RESULT($MICO_URGENCY)

#
# Now we know if we want mico or not, only go further if
# it's wanted.
#
if test x$MICO_URGENCY = xrequired -o  x$MICO_URGENCY = xoptional; then
    #
    # If not specified by caller, search in standard places
    #
    if test -z "$MICOSETUP" ; then
      AC_PATH_PROG(MICOSETUP, mico-setup.sh, , $prefix/lib:/usr/local/lib:/usr/lib)
    fi
    if test -z "$MICOSETUP" ; then
        if test x$MICO_URGENCY = xrequired ; then
            AC_MSG_ERROR("mico setup file mico-setup.sh not found")
        else
            AC_MSG_RESULT("mico setup file mico-setup.sh not found")
        fi
    else
        # source mico-setup.sh
        # Collect mico version number.
        # But be carefull - mico-setup.sh
        # unsets prefix and exec_prefix so save them first
        sh -c ". $MICOSETUP; echo MICOVERSION=\${MICOVERSION}; echo MICOSHAREDDIR=\${MICOSHAREDDIR}; echo MICODIR=\${MICODIR}" > conftest.mico-setup

        eval `cat conftest.mico-setup`

        if test -z "$MICOVERSION" ; then
            AC_MSG_ERROR("panic: could not read the MICOVERSION variable");
        fi
        #
        # Check that mico version matches requested version or above
        #
        if test -n "$MICO_REQUEST_VERSION" ; then
            changequote(<<, >>)dnl
            MICO_MAJOR=`expr $MICOVERSION : '\([0-9]*\)\.[0-9]*\.[0-9]*'`
            MICO_MINOR=`expr $MICOVERSION : '[0-9]*\.\([0-9]*\)\.[0-9]*'`
            MICO_RELEASE=`expr $MICOVERSION : '[0-9]*\.[0-9]*\.\([0-9]*\)'`
            MICO_REQUEST_MAJOR=`expr $MICO_REQUEST_VERSION : '\([0-9]*\)\.[0-9]*\.[0-9]*'`
            MICO_REQUEST_MINOR=`expr $MICO_REQUEST_VERSION : '[0-9]*\.\([0-9]*\)\.[0-9]*'`
            MICO_REQUEST_RELEASE=`expr $MICO_REQUEST_VERSION : '[0-9]*\.[0-9]*\.\([0-9]*\)'`
            changequote([, ])dnl

            AC_MSG_CHECKING(mico version >= $MICO_REQUEST_VERSION)

            version_ok=yes
            for verpair in "$MICO_MAJOR:$MICO_REQUEST_MAJOR" "$MICO_MINOR:$MICO_REQUEST_MINOR" "$MICO_RELEASE:$MICO_REQUEST_RELEASE"; do
                if eval "test `echo $verpair | sed 's,:, -gt ,'`"; then
                    break
                fi
                if eval "test `echo $verpair | sed 's,:, -lt ,'`"; then
                    version_ok=no
                    break
                fi
            done
            if test  x$version_ok = xno; then
                AC_MSG_RESULT(no)
                AC_MSG_ERROR(mico version is $MICOVERSION)
            else
                AC_MSG_RESULT(yes)
            fi
        fi
        #
        # look for mico-c++ and scan it for a string match
        # to "-I*ministl"
        #
        AC_PATH_PROG(MICO_CXX, mico-c++, , ${MICODIR}/bin:${MICOSHAREDDIR}/bin)
        if test -z "$MICO_CXX"; then
            AC_MSG_ERROR("panic: cannot locate and scan the mico script mico-c++")
        fi

        AC_MSG_CHECKING(if mico was compiled using ministl)

        if test -z "`grep '\-I.*ministl' $MICO_CXX`"; then
            MICO_USE_MINISTL=0
            AC_MSG_RESULT(no)
        else
            MICO_USE_MINISTL=1
            AC_MSG_RESULT(yes)
        fi
        #
        # look for mico-ld and scan it for -l* libraries
        # needed to link with mico
        #
        AC_PATH_PROG(MICO_LD, mico-ld, , ${MICODIR}/bin:${MICOSHAREDDIR}/bin)

        if test -z "$MICO_LD"; then
            AC_MSG_ERROR("panic: cannot locate and scan the mico script mico-ld")
        fi

        AC_MSG_CHECKING(mico dependencies)

        micodeps=
        for item in `grep -- -l $MICO_LD | tr '"' ' '` ; do
            case $item in
                -l*) micodeps="$micodeps $item" ;;
            esac
        done

        if test -z "$micodeps" ;then
            AC_MSG_RESULT(none)
        else
            AC_MSG_RESULT($micodeps)
        fi

        AC_MSG_CHECKING(how to link with the mico library)
        MICO_LIB="-L${MICODIR}/lib -lmico${MICOVERSION} $micodeps"
        AC_MSG_RESULT($MICO_LIB)

        MICO_INCLUDE="-I${MICOSHAREDDIR}/include"

        AC_SUBST(MICODIR)
        AC_SUBST(MICOVERSION)
        AC_SUBST(MICOSHAREDDIR)
        AC_SUBST(MICO_INCLUDE)
        AC_SUBST(MICO_LIB)
        AC_SUBST(MICO_USE_MINISTL)
    fi
fi
])

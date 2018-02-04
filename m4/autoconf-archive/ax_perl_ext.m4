# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_perl_ext.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PERL_EXT
#
# DESCRIPTION
#
#   Fetches the linker flags and C compiler flags for compiling and linking
#   Perl binary extensions.  The macro substitutes PERL_EXT_PREFIX,
#   PERL_EXT_INC, PERL_EXT_LIB, PERL_EXT_CPPFLAGS, PERL_EXT_LDFLAGS and
#   PERL_EXT_DLEXT variables if Perl executable was found.  It also checks
#   the same variables before trying to retrieve them from the Perl
#   configuration.
#
#     PERL_EXT_PREFIX: top-level perl installation path (--prefix)
#     PERL_EXT_INC: XS include directory
#     PERL_EXT_LIB: Perl extensions destination directory
#     PERL_EXT_CPPFLAGS: C preprocessor flags to compile extensions
#     PERL_EXT_LDFLAGS: linker flags to build extensions
#     PERL_EXT_DLEXT: extensions suffix for perl modules (e.g. ".so")
#
#   Examples:
#
#     AX_PERL_EXT
#     if test x"$PERL" = x; then
#        AC_ERROR(["cannot find Perl"])
#     fi
#
# LICENSE
#
#   Copyright (c) 2011 Stanislav Sedov <stas@FreeBSD.org>
#   Copyright (c) 2014 Thomas Klausner <tk@giga.or.at>
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#   1. Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
#   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
#   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
#   THE POSSIBILITY OF SUCH DAMAGE.

#serial 3

AC_DEFUN([AX_PERL_EXT],[

        #
        # Check if perl executable exists.
        #
        AC_PATH_PROGS(PERL, ["${PERL-perl}"], [])

        if test -n "$PERL" ; then

                #
                # Check for Perl prefix.
                #
                AC_ARG_VAR(PERL_EXT_PREFIX, [Perl PREFIX])
                AC_MSG_CHECKING([for Perl prefix])
                if test -z "$PERL_EXT_PREFIX" ; then
                        [PERL_EXT_PREFIX=`$PERL -MConfig -e 'print $Config{prefix};'`];
                fi
                AC_MSG_RESULT([$PERL_EXT_PREFIX])
                AC_SUBST(PERL_EXT_PREFIX)

                #
                # Check for Perl extensions include path.
                #
                AC_ARG_VAR(PERL_EXT_INC, [Directory to include XS headers from])
                AC_MSG_CHECKING([for Perl extension include path])
                if test -z "$PERL_EXT_INC" ; then
                        [PERL_EXT_INC=`$PERL -MConfig -e 'print $Config{archlibexp}, "/CORE";'`];
                fi
                AC_MSG_RESULT([$PERL_EXT_INC])
                AC_SUBST(PERL_EXT_INC)

                #
                # Check for the extensions target directory.
                #
                AC_ARG_VAR(PERL_EXT_LIB, [Directory to install perl files into])
                AC_MSG_CHECKING([for Perl extension target directory])
                if test -z "$PERL_EXT_LIB" ; then
                        [PERL_EXT_LIB=`$PERL -MConfig -e 'print $Config{sitearch};'`];
                fi
                AC_MSG_RESULT([$PERL_EXT_LIB])
                AC_SUBST(PERL_EXT_LIB)

                #
                # Check for Perl CPP flags.
                #
                AC_ARG_VAR(PERL_EXT_CPPFLAGS, [CPPFLAGS to compile perl extensions])
                AC_MSG_CHECKING([for Perl extensions C preprocessor flags])
                if test -z "$PERL_EXT_CPPFLAGS" ; then
                        [PERL_EXT_CPPFLAGS=`$PERL -MConfig -e 'print $Config{cppflags};'`];
                fi
                AC_MSG_RESULT([$PERL_EXT_CPPFLAGS])
                AC_SUBST(PERL_EXT_CPPFLAGS)

                #
                # Check for Perl extension link flags.
                #
                AC_ARG_VAR(PERL_EXT_LDFLAGS, [LDFLAGS to build perl extensions])
                AC_MSG_CHECKING([for Perl extensions linker flags])
                if test -z "$PERL_EXT_LDFLAGS" ; then
                        [PERL_EXT_LDFLAGS=`$PERL -MConfig -e 'print $Config{lddlflags};'`];
                fi
                # Fix LDFLAGS for OS X.  We don't want any -arch flags here, otherwise
                # linking will fail.  Also, OS X Perl LDFLAGS contains "-arch ppc" which
                # is not supported by XCode anymore.
                case "${host}" in
                *darwin*)
                        PERL_EXT_LDFLAGS=`echo ${PERL_EXT_LDFLAGS} | sed -e "s,-arch [[^ ]]*,,g"`
                        ;;
                esac
                AC_MSG_RESULT([$PERL_EXT_LDFLAGS])
                AC_SUBST(PERL_EXT_LDFLAGS)

                #
                # Check for Perl dynamic library extension.
                #
                AC_ARG_VAR(PERL_EXT_DLEXT, [Perl dynamic library extension])
                AC_MSG_CHECKING([for Perl dynamic library extension])
                if test -z "$PERL_EXT_DLEXT" ; then
                        [PERL_EXT_DLEXT=`$PERL -MConfig -e 'print ".", $Config{'dlext'};'`];
                fi
                AC_MSG_RESULT([$PERL_EXT_DLEXT])
                AC_SUBST(PERL_EXT_DLEXT)
        fi
])

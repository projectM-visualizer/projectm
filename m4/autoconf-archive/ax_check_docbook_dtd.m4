# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_check_docbook_dtd.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_DOCBOOK_DTD([dtd-version])
#
# DESCRIPTION
#
#   Check for access to a docbook DTD of a particular revision. This macro
#   can be used for multiple versions within the same script.
#
#   Input:
#
#   $1 is the version of docbook to search for; default 'current'.
#
#   Output:
#
#   $HAVE_DOCBOOK_DTD_VERS will be set to 'yes' or 'no' depending on the
#   results of the test, where VERS is $1, with '_' substituted for '.'
#   $HAVE_DOCBOOK_DTD will also be set to the same value.
#
#   Example:
#
#    AX_CHECK_DOCBOOK_DTD(4.3)
#    if test "x$HAVE_DOCBOOK_DTD_4_3" = "xyes"; then
#    ...
#
# LICENSE
#
#   Copyright (c) 2008 Zmanda Inc. <http://www.zmanda.com/>
#   Copyright (c) 2008 Dustin J. Mitchell <dustin@zmanda.com>
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

AU_ALIAS([AC_CHECK_DOCBOOK_DTD], [AX_CHECK_DOCBOOK_DTD])
AC_DEFUN([AX_CHECK_DOCBOOK_DTD],
[
    AC_REQUIRE([AX_PROG_XSLTPROC])

    dnl define a temporary variable for the version, so this macro can be
    dnl used with multiple versions
    define([_VERS], $1)
    ifelse(_VERS, [], [define([_VERS], [current])])
    define([ac_cv_docbook_dtd_VERS], patsubst([ac_cv_docbook_dtd_]_VERS, [\.], [_]))
    define([HAVE_DOCBOOK_DTD_VERS], patsubst([HAVE_DOCBOOK_DTD_]_VERS, [\.], [_]))

    AC_CACHE_CHECK([for Docbook DTD version ]_VERS, [ac_cv_docbook_dtd_VERS],
    [
        ac_cv_docbook_dtd_VERS=no
        if test -n "$XSLTPROC"; then
            MY_XSLTPROC_FLAGS=`echo "" $XSLTPROC_FLAGS|sed -e s/--novalid//g`
            cat <<EOF >conftest.xml
<?xml version="1.0" encoding='ISO-8859-1'?>
<!DOCTYPE book PUBLIC "-//OASIS//DTD DocBook XML V[]_VERS//EN" "http://www.oasis-open.org/docbook/xml/_VERS/docbookx.dtd">
<book id="empty">
</book>
EOF
            echo "Trying '$XSLTPROC $MY_XSLTPROC_FLAGS conftest.xml'" >&AS_MESSAGE_LOG_FD
            $XSLTPROC $MY_XSLTPROC_FLAGS conftest.xml >conftest.out 2>&1
            if test "$?" = 0 -o "$?" = 5; then
                # failing to load the DTD is just a warning, so check for it in the output.
                if grep 'warning: failed to load external entity' conftest_out >/dev/null 2>&1; then
                    : # no good..
                else
                    ac_cv_docbook_dtd_VERS=yes
                fi
            fi
            cat conftest.out >&AS_MESSAGE_LOG_FD

            rm -f conftest.xml conftest.out
        fi
    ])

    HAVE_DOCBOOK_DTD_VERS="$ac_cv_docbook_dtd_VERS"
    HAVE_DOCBOOK_DTD=HAVE_DOCBOOK_DTD_VERS
    undefine([_VERS])
])

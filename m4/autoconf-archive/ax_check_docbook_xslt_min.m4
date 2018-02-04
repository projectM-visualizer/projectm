# ==============================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_check_docbook_xslt_min.html
# ==============================================================================
#
# SYNOPSIS
#
#   AX_CHECK_DOCBOOK_XSLT_MIN(min-xslt-version)
#
# DESCRIPTION
#
#   Check that the 'current' version of docbook is at least version
#   min-xslt-version. If the test is successful,
#   $DOCBOOK_XSLT_CURRENT_VERSION will be set to the current docbook
#   version; if not, it will be set to 'no'.
#
#   Example:
#
#    AX_CHECK_DOCBOOK_XSLT_MIN(1.72.0)
#    if test "x$DOCBOOK_XSLT_CURRENT_VERSION" = "xno"; then
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

AU_ALIAS([AC_CHECK_DOCBOOK_XSLT_MIN], [AX_CHECK_DOCBOOK_XSLT_MIN])
AC_DEFUN([AX_CHECK_DOCBOOK_XSLT_MIN],
[
    AC_REQUIRE([AX_PROG_XSLTPROC])

    AC_CACHE_CHECK([for current Docbook XSLT version], [ac_cv_docbook_xslt_current_version],
    [
        ac_cv_docbook_xslt_current_version=no

        if test -n "$XSLTPROC"; then
            cat >conftest.xsl <<EOF
                <xsl:stylesheet
                    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                    xmlns:fm="http://freshmeat.net/projects/freshmeat-submit/"
                    version="1.0">
                    <xsl:output method="text"/>
                    <xsl:template match="fm:project/fm:Version">
                        <xsl:value-of select="." />
                    </xsl:template>
                    <!-- do nothing with any other text -->
                    <xsl:template match="text()"/>
                </xsl:stylesheet>
EOF
            echo "Trying '$XSLTPROC $XSLTPROC_FLAGS http://docbook.sourceforge.net/release/xsl/current/VERSION' with input:" >&AS_MESSAGE_LOG_FD
            echo "====" >&AS_MESSAGE_LOG_FD
            cat conftest.xsl >&AS_MESSAGE_LOG_FD
            echo "====" >&AS_MESSAGE_LOG_FD

            ac_cv_docbook_xslt_current_version=`$XSLTPROC $XSLTPROC_FLAGS conftest.xsl http://docbook.sourceforge.net/release/xsl/current/VERSION 2>&AS_MESSAGE_LOG_FD`

            if test "$?" != 0; then
                ac_cv_docbook_xslt_current_version='no'
            fi

            rm conftest.xsl
        fi
    ])

    DOCBOOK_XSLT_CURRENT_VERSION="$ac_cv_docbook_xslt_current_version"
    AC_MSG_CHECKING([whether Docbook XSLT version is $1 or newer])

    if test x"$DOCBOOK_XSLT_CURRENT_VERSION" = x"no"; then
        AC_MSG_RESULT([no])
    else
        AX_COMPARE_VERSION([$DOCBOOK_XSLT_CURRENT_VERSION], [lt], [$1], [
            # version is less than required, so mark it as "no"
            DOCBOOK_XSLT_CURRENT_VERSION=no
        ])

        if test x"$DOCBOOK_XSLT_CURRENT_VERSION" = x"no"; then
            AC_MSG_RESULT([no])
        else
            AC_MSG_RESULT([yes ($DOCBOOK_XSLT_CURRENT_VERSION)])
        fi
    fi
])

# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_prog_httpd.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_HTTPD
#
# DESCRIPTION
#
#   Check for Apache's 'httpd', let script continue if exists & works, pops
#   up error message if not.
#
#   Testing of functionality is by checking its compile settings
#
#   Besides checking existence, this macro also set these environment
#   variables upon completion:
#
#     HTTPD                    = which httpd
#     HTTPD_ROOT               = Apache's root directory, specified when compiled / run with -d option
#     HTTPD_SERVER_ROOT        = Directory for Apache's essential files, e.g. access logs / error logs / modules / scripts.
#     HTTPD_SERVER_CONFIG_FILE = Full-path of the 'httpd.conf' file
#     HTTPD_USER               = Which user that httpd runs as
#     HTTPD_GROUP              = Which group that httpd runs as
#     HTTPD_DOC_HOME           = Document directory, taken as the first DocumentRoot path found in httpd.conf
#     HTTPD_SCRIPT_HOME        = CGI script directory, taken as the first ScriptAlias path found in httpd.conf
#
# LICENSE
#
#   Copyright (c) 2008 Gleen Salmon <gleensalmon@yahoo.com>
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

AU_ALIAS([AC_PROG_HTTPD], [AX_PROG_HTTPD])
AC_DEFUN([AX_PROG_HTTPD],[
AC_REQUIRE([AC_EXEEXT])dnl
AC_PATH_PROG(HTTPD, httpd$EXEEXT, nocommand)
if test "$HTTPD" = nocommand; then
        AC_MSG_ERROR([httpd not found in $PATH])
fi
HTTPD_ROOT=`httpd -V | grep HTTPD_ROOT | sed 's/^.*HTTPD_ROOT[[[:blank:]]]*=[[[:blank:]]]*"\(.*\)"$/\1/'`
HTTPD_SERVER_CONFIG_FILE=`httpd -V | grep SERVER_CONFIG_FILE | sed 's/^.*SERVER_CONFIG_FILE[[[:blank:]]]*=[[[:blank:]]]*"\(.*\)"$/\1/'`
if echo $HTTPD_SERVER_CONFIG_FILE | grep ^[[^/]] > /dev/null; then
        HTTPD_SERVER_CONFIG_FILE=$HTTPD_ROOT/$HTTPD_SERVER_CONFIG_FILE
fi
SERVER_ROOT_PATTERN='^[[[:blank:]]]*ServerRoot[[[:blank:]]][[[:blank:]]]*"\([[^"]]*\)"$'
HTTPD_USER_PATTERN='^User[[[:blank:]]][[[:blank:]]]*\([[^[:blank:]]][[^[:blank:]]]*\)$'
HTTPD_GROUP_PATTERN='^Group[[[:blank:]]][[[:blank:]]]*\([[^[:blank:]]][[^[:blank:]]]*\)$'
DOCUMENT_ROOT_PATTERN='^[[[:blank:]]]*DocumentRoot[[[:blank:]]][[[:blank:]]]*"\([[^"]]*\)"$'
SCRIPT_ALIAS_PATTERN='^[[[:blank:]]]*ScriptAlias[[[:blank:]]][[[:blank:]]]*[[^[:blank:]]][[^[:blank:]]]*[[[:blank:]]][[[:blank:]]]*"\([[^"]]*\)"$'
AC_CHECK_FILE($HTTPD_SERVER_CONFIG_FILE,
        [HTTPD_SERVER_ROOT=`grep $SERVER_ROOT_PATTERN $HTTPD_SERVER_CONFIG_FILE | head -n 1 | sed "s/$SERVER_ROOT_PATTERN/\1/" | sed s/[[/]]$//`;
                HTTPD_USER=`grep $HTTPD_USER_PATTERN $HTTPD_SERVER_CONFIG_FILE | sed "s/$HTTPD_USER_PATTERN/\1/"`;
                HTTPD_GROUP=`grep $HTTPD_GROUP_PATTERN $HTTPD_SERVER_CONFIG_FILE | sed "s/$HTTPD_GROUP_PATTERN/\1/"`;
                HTTPD_DOC_HOME=`grep $DOCUMENT_ROOT_PATTERN $HTTPD_SERVER_CONFIG_FILE | head -n 1 | sed "s/$DOCUMENT_ROOT_PATTERN/\1/" | sed s/[[/]]$//`;
                HTTPD_SCRIPT_HOME=`grep $SCRIPT_ALIAS_PATTERN $HTTPD_SERVER_CONFIG_FILE | head -n 1 | sed "s/$SCRIPT_ALIAS_PATTERN/\1/" | sed s/[[/]]$//`],
        AC_MSG_ERROR([httpd server-config-file (detected as $HTTPD_SERVER_CONFIG_FILE by httpd -V) cannot be found]))dnl
])

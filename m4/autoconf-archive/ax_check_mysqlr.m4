# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_check_mysqlr.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_MYSQLR
#
# DESCRIPTION
#
#   First check if mysqlconfig exists. It fails if mysqlconfig is not in
#   path. Then it checks for the libraries and replaces -lmysqlclient
#   statement with -lmysqlclient_r statement, to enable threaded client
#   library.
#
#   The following are exported environment variables:
#
#     MYSQL_LIBS
#     MYSQL_CFLAGS
#
# LICENSE
#
#   Copyright (c) 2008 Can Bican <bican@yahoo.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([AC_CHECK_MYSQLR], [AX_CHECK_MYSQLR])
AC_DEFUN([AX_CHECK_MYSQLR],[
AC_PATH_PROG(mysqlconfig,mysql_config)
if test [ -z "$mysqlconfig" ]
then
    AC_MSG_ERROR([mysql_config executable not found])
else
    AC_MSG_CHECKING(mysql libraries)
    MYSQL_LIBS=`${mysqlconfig} --libs | sed -e \
    's/-lmysqlclient /-lmysqlclient_r /' -e 's/-lmysqlclient$/-lmysqlclient_r/'`
    AC_MSG_RESULT($MYSQL_LIBS)
    AC_MSG_CHECKING(mysql includes)
    MYSQL_CFLAGS=`${mysqlconfig} --cflags`
    AC_MSG_RESULT($MYSQL_CFLAGS)
fi
])

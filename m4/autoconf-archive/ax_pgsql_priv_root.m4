# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_pgsql_priv_root.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PGSQL_PRIV_ROOT(DB, USER, [HOST], [PASSWORD], [ACTION_IF_FAILED], [ACTION_IF_OK])
#
# DESCRIPTION
#
#   This macro checks wether the given PostgreSQL user has root privileges
#   (can create and drop databases) It is recommended to first call
#   AX_CHECK_PGSQL_DB, this makes it easier to locate the cause of error.
#   The macro AX_PROG_PGCLIENT is required by this one.
#
#   The variable $pgclient_root_call is set for later use in Makefiles, if
#   you'd like to make use of this, you must do
#
#     AC_SUBST(pgclient_root_call)
#
#   after having called AX_CHECK_PGSQL_PRIV_ROOT. You can then do something
#   like the following in your Makefile.am:
#
#     @pgclient_root_call@ -f file.sql
#
#   If you want the user to set the data, you should support something like
#   these configure options:
#
#     AC_ARG_WITH(pgsql-host,
#             [  --with-pgsql-host=HOST               server is running on HOST @<:@local socket@:>@],
#             [pg_host=$withval], [pg_host=])
#
#     AC_ARG_WITH(pgsql-db,
#             [  --with-pgsql-db=DBNAME               use database DBNAME @<:@test@:>@],
#             [pg_db=$withval], [pg_db=test])
#
#     AC_ARG_WITH(pgsql-root-user,
#             [  --with-pgsql-root-user=USER          use user USER, must have root (all) privileges @<:@postgres@:>@],
#             [pg_root_user=$withval], [pg_root_user=postgres])
#
#     AC_ARG_WITH(pgsql-password,
#             [  --with-pgsql-password=PASSWORD       use password PASSWORD @<:@none@:>@],
#             [pg_password=$withval], [pg_password=""])
#
#   You can then call the macro like this:
#
#     AX_CHECK_PGSQL_PRIV_ROOT([$pg_db], [$pg_root_user], [$pg_host], [$pg_password], [AC_MSG_ERROR([We need root privileges on database!])])
#
# LICENSE
#
#   Copyright (c) 2008 Moritz Sinn <moritz@freesources.org>
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

#serial 5

AC_DEFUN([AX_CHECK_PGSQL_PRIV_ROOT], [
AC_REQUIRE([AX_PROG_PGCLIENT])dnl
AC_REQUIRE([AX_CHECK_PGSQL_DB])dnl
AC_MSG_CHECKING([if PostgreSQL user $2 has root privileges])

pgclient_root_call="$pgclient"

if test "x$1" != "x"; then
        pgclient_root_call="$pgclient_root_call dbname=$1";
fi
if test "x$2" != "x"; then
        pgclient_root_call="$pgclient_root_call user=$2";
fi
if test "x$3" != "x"; then
        pgclient_root_call="$pgclient_root_call host=$3";
fi
if test "x$4" != "x"; then
        pgclient_root_call="$pgclient_root_call password=$4";
fi

testdb="test`date +%s`"
echo "CREATE DATABASE $testdb; DROP DATABASE $testdb;" | $pgclient_root_call  > /dev/null 2>&1
if test "x$?" = "x0"; then
        AC_MSG_RESULT([yes])
        $6
else
        AC_MSG_RESULT([no])
        $5
fi;
])dnl

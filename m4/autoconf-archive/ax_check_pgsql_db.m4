# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_check_pgsql_db.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_PGSQL_DB([DB], [USER], [HOST], [PASSWORD], [ACTION_IF_FAILED], [ACTION_IF_OK])
#
# DESCRIPTION
#
#   This macro checks wether we can connect to a PostgreSQL server with the
#   given data. The macro AX_PROG_PGCLIENT is required by this one. The
#   variable $pgclient_call is set for later use in Makefiles, if you'd like
#   to make use of this, you must do
#
#     AC_SUBST(pgclient_call)
#
#   after having called AX_CHECK_PGSQL_DB. You can then do something like
#   the following in your Makefile.am:
#
#     @pgclient_call@ -f file.sql
#
#   If you want the user to set the data, you should support something like
#   these configure options:
#
#     AC_ARG_WITH(pgsql-host,
#             [  --with-pgsql-host=HOST               server is running on HOST @<:@local socket@:>@],
#             [pg_host=$withval], [pg_host=])
#
#     AC_ARG_WITH(pgsql-db,
#             [  --with-pgsql-db=DATABASE             use DATABASE @<:@tarantoola@:>@],
#             [pg_db=$withval], [pg_db=tarantoola])
#
#     AC_ARG_WITH(pgsql-user,
#             [  --with-pgsql-user=USER               use USER @<:@postgres@:>@],
#             [pg_user=$withval], [pg_user=postgres])
#
#     AC_ARG_WITH(pgsql-password,
#             [  --with-pgsql-password=PASSWORD       use PASSWORD @<:@none@:>@],
#             [pg_password=$withval], [pg_password=""])
#
#   You can then call the macro like this:
#
#     AX_CHECK_PGSQL_DB([$pg_db], [$pg_user], [$pg_host], [$pg_password], [AC_MSG_ERROR([We need a database connection!])])
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

#serial 6

AU_ALIAS([MS_CHECK_PGSQL_DB], [AX_CHECK_PGSQL_DB])
AC_DEFUN([AX_CHECK_PGSQL_DB], [
AC_REQUIRE([AX_PROG_PGCLIENT])
AC_MSG_CHECKING([for PostgreSQL db $1 (user: $2, host: $3)])

pgclient_call="$pgclient"

if test "x$1" != "x"; then
        pgclient_call="$pgclient_call dbname=$1";
fi
if test "x$2" != "x"; then
        pgclient_call="$pgclient_call user=$2";
fi
if test "x$3" != "x"; then
        pgclient_call="$pgclient_call host=$3";
fi
if test "x$4" != "x"; then
        pgclient_call="$pgclient_call password=$4";
fi

$pgclient_call -c 'SELECT 1' > /dev/null 2>&1
if test "x$?" = "x0"; then
        AC_MSG_RESULT([yes])
        $6
else
        AC_MSG_RESULT([no])
        $5
fi;
])dnl

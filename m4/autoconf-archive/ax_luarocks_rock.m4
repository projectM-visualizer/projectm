# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_luarocks_rock.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LUAROCKS_ROCK([ROCK])
#
# DESCRIPTION
#
#   Checks for a rock, and fails if it is not installed.
#
#   Example usage:
#
#     AX_LUAROCKS_ROCK(stdlib)
#
#   Note: use of this macro is not normally recommended. Normally, LuaRocks
#   should be used to drive the build system, and it takes care of rock
#   dependencies. Use this macro only if LuaRocks cannot be used at the top
#   level, for example, in a build system that uses Lua only incidentally.
#
# LICENSE
#
#   Copyright (c) 2016 Reuben Thomas <rrt@sc3d.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_LUAROCKS_ROCK],[
    # Make sure we have luarocks
    if test -z "$LUAROCKS"; then
       AX_WITH_PROG(LUAROCKS,luarocks)
       if test -z "$LUAROCKS"; then
           AC_MSG_ERROR([can't find luarocks])
       fi
    fi

    AC_PREREQ([2.61])

    pushdef([ROCK],$1)
    AC_MSG_CHECKING(whether LuaRock ROCK is installed)
    AS_IF(["$LUAROCKS" show ROCK > /dev/null],[
        AC_MSG_RESULT(yes)
    ],[
        AC_MSG_FAILURE([LuaRock ROCK not found])
    ])

    popdef([ROCK])
])

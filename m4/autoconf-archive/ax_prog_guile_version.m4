# ===========================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_prog_guile_version.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_GUILE_VERSION([VERSION],[ACTION-IF-TRUE],[ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   Makes sure that guile supports the version indicated. If true the shell
#   commands in ACTION-IF-TRUE are executed. If not the shell commands in
#   ACTION-IF-FALSE are run. Note if $GUILE is not set (for example by
#   running AC_CHECK_PROG or AC_PATH_PROG) the macro will fail.
#
#   Example:
#
#     AC_PATH_PROG([GUILE],[guile])
#     AC_PROG_GUILE_VERSION([1.6.0],[ ... ],[ ... ])
#
#   This will check to make sure that the guile you have supports at least
#   version 1.6.0.
#
#   NOTE: This macro uses the $GUILE variable to perform the check.
#   AX_WITH_GUILE can be used to set that variable prior to running this
#   macro. The $GUILE_VERSION variable will be valorized with the detected
#   version.
#
# LICENSE
#
#   Copyright (c) 2009 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 11

AC_DEFUN([AX_PROG_GUILE_VERSION],[
    AC_REQUIRE([AC_PROG_SED])

    AS_IF([test -n "$GUILE"],[
        ax_guile_version="$1"

        AC_MSG_CHECKING([for guile version])
        changequote(<<,>>)
        guile_version=`$GUILE -q --version 2>&1 | head -n 1 | $SED -e 's/.* \([0-9]*\.[0-9]*\.[0-9]*\)/\1/'`
        changequote([,])
        AC_MSG_RESULT($guile_version)

	AC_SUBST([GUILE_VERSION],[$guile_version])

        AX_COMPARE_VERSION([$ax_guile_version],[le],[$guile_version],[
	    :
            $2
        ],[
	    :
            $3
        ])
    ],[
        AC_MSG_WARN([could not find the guile interpreter])
        $3
    ])
])

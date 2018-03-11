# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_at_check_pattern.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_AT_CHECK_PATTERN(COMMANDS, [STATUS], [STDOUT-RE], [STDERR-RE], [RUN-IF-FAIL], [RUN-IF-PASS])
#   AX_AT_DIFF_PATTERN(PATTERN-FILE, TEST-FILE, [STATUS=0], [DIFFERENCES])
#
# DESCRIPTION
#
#   AX_AT_CHECK_PATTERN() executes a test similar to AT_CHECK(), except that
#   stdout and stderr are awk regular expressions (REs).
#
#   NOTE: as autoconf uses [] for quoting, the use of [brackets] in the RE
#   arguments STDOUT-RE and STDERR-RE can be awkward and require careful
#   extra quoting, or quadrigraphs '@<:@' (for '[') and '@:>@' (for ']').
#
#   awk is invoked via $AWK, which defaults to "awk" if unset or empty.
#
#   Implemented using AT_CHECK() with a custom value for $at_diff that
#   invokes diff with an awk post-processor.
#
#   AX_AT_DIFF_PATTERN() checks that the PATTERN-FILE applies to TEST-FILE.
#   If there are differences, STATUS will be 1 and they should be
#   DIFFERENCES.
#
# LICENSE
#
#   Copyright (c) 2013-2014 Luke Mewburn <luke@mewburn.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 2

m4_defun([_AX_AT_CHECK_PATTERN_PREPARE], [dnl
dnl Can't use AC_PROG_AWK() in autotest.
AS_VAR_IF([AWK], [], [AWK=awk])

AS_REQUIRE_SHELL_FN([ax_at_diff_pattern],
  [AS_FUNCTION_DESCRIBE([ax_at_diff_pattern], [PATTERN OUTPUT],
    [Diff PATTERN OUTPUT and elide change lines where the RE pattern matches])],
[diff "$[]1" "$[]2" | $AWK '
BEGIN { exitval=0 }

function mismatch()
{
	print mode
	for (i = 0; i < lc; i++) {
		print ll[[i]]
	}
	print "---"
	for (i = 0; i < rc; i++) {
		print rl[[i]]
	}
	mode=""
	exitval=1
}

$[]1 ~ /^[[0-9]]+(,[[0-9]]+)?[[ad]][[0-9]]+(,[[0-9]]+)?$/ {
	print
	mode=""
	exitval=1
	next
}

$[]1 ~ /^[[0-9]]+(,[[0-9]]+)?[[c]][[0-9]]+(,[[0-9]]+)?$/ {
	mode=$[]1
	lc=0
	rc=0
	next
}

mode == "" {
	print $[]0
	next
}

$[]1 == "<" {
	ll[[lc]] = $[]0
	lc = lc + 1
	next
}

$[]1 == "---" {
	next
}

$[]1 == ">" {
	rl[[rc]] = $[]0
	rc = rc + 1
	if (rc > lc) {
		mismatch()
		next
	}
	pat = "^" substr(ll[[rc-1]], 2) "$"
	str = substr($[]0, 2)
	if (str !~ pat) {
		mismatch()
	}
	next
}
{
	print "UNEXPECTED LINE: " $[]0
	exit 10
}

END { exit exitval }
'])dnl ax_at_diff_pattern
])dnl _AX_AT_CHECK_PATTERN_PREPARE


m4_defun([AX_AT_CHECK_PATTERN], [dnl
AS_REQUIRE([_AX_AT_CHECK_PATTERN_PREPARE])
_ax_at_check_pattern_prepare_original_at_diff="$at_diff"
at_diff='ax_at_diff_pattern'
AT_CHECK($1, $2, $3, $4,
        [at_diff="$_ax_at_check_pattern_prepare_original_at_diff";]$5,
        [at_diff="$_ax_at_check_pattern_prepare_original_at_diff";]$6)

])dnl AX_AT_CHECK_PATTERN


m4_defun([AX_AT_DIFF_PATTERN], [dnl
AS_REQUIRE([_AX_AT_CHECK_PATTERN_PREPARE])
AT_CHECK([ax_at_diff_pattern $1 $2], [$3], [$4])
])dnl AX_AT_CHECK_PATTERN

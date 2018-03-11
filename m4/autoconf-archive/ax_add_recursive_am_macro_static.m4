# =====================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_add_recursive_am_macro_static.html
# =====================================================================================
#
# SYNOPSIS
#
#   AX_ADD_RECURSIVE_AM_MACRO_STATIC([TARGET],[RULE])
#
# DESCRIPTION
#
#   Adds the specified rule to AMINCLUDE_STATIC along with a
#   TARGET-recursive rule that will call TARGET for the current directory
#   and TARGET-am recursively for each subdirectory.
#
# LICENSE
#
#   Copyright (c) 2009 Tom Howard <tomhoward@users.sf.net>
#   Copyright (c) 2009 Allan Caffee <allan.caffee@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AC_DEFUN([AX_ADD_RECURSIVE_AM_MACRO_STATIC],[
  AX_ADD_AM_MACRO_STATIC([
$1-recursive:
	@set fnord ${AX_DOLLAR}${AX_DOLLAR}MAKEFLAGS; amf=${AX_DOLLAR}${AX_DOLLAR}2; \\
	dot_seen=no; \\
	list='${AX_DOLLAR}(SUBDIRS)'; for subdir in ${AX_DOLLAR}${AX_DOLLAR}list; do \\
	  echo \"Making $1 in ${AX_DOLLAR}${AX_DOLLAR}subdir\"; \\
	  if test \"${AX_DOLLAR}${AX_DOLLAR}subdir\" = \".\"; then \\
	    dot_seen=yes; \\
	    local_target=\"$1-am\"; \\
	  else \\
	    local_target=\"$1\"; \\
	  fi; \\
	  (cd ${AX_DOLLAR}${AX_DOLLAR}subdir && ${AX_DOLLAR}(MAKE) ${AX_DOLLAR}(AM_MAKEFLAGS) ${AX_DOLLAR}${AX_DOLLAR}local_target) \\
	   || case \"${AX_DOLLAR}${AX_DOLLAR}amf\" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \\
	done; \\
	if test \"${AX_DOLLAR}${AX_DOLLAR}dot_seen\" = \"no\"; then \\
	  ${AX_DOLLAR}(MAKE) ${AX_DOLLAR}(AM_MAKEFLAGS) \"$1-am\" || exit 1; \\
	fi; test -z \"${AX_DOLLAR}${AX_DOLLAR}fail\"

$2
])
])

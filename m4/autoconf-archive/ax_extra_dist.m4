# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_extra_dist.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_EXTRA_DIST
#
# DESCRIPTION
#
#   Allow support for custom dist targets.
#
#   To add custom dist targets, you must create a dist-<TYPE> target within
#   your Makefile.am, where <TYPE> is the name of the dist and then add
#   <TYPE> to EXTRA_SRC_DISTS or EXTRA_BIN_DISTS. For example:
#
#     dist-foobar:
#      <rules for making the foobar dist>
#
#     EXTRA_BIN_DISTS += foobar
#
#   You can then build all the src dist targets by running:
#
#     make dist-src
#
#   You can build all the binary dist targets by running:
#
#     make dist-bin
#
#   and you can build both the src and dist targets by running:
#
#     make all-dist
#
# LICENSE
#
#   Copyright (c) 2009 Tom Howard <tomhoward@users.sf.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 12

AC_DEFUN([AX_EXTRA_DIST],
[
AC_MSG_NOTICE([adding custom dist support])
AM_CONDITIONAL(USING_AX_EXTRA_DIST, [true])
AX_ADD_AM_MACRO_STATIC([
EXTRA_SRC_DISTS =
EXTRA_BIN_DISTS =
dist-src-extra:
	@echo \"Making custom src targets...\"
	@cd \$(top_builddir); \\
	list='\$(EXTRA_SRC_DISTS)'; \\
	for dist in \$\$list; do \\
	    \$(MAKE) \$(AM_MAKEFLAGS) dist-\$\$dist; \\
	done

dist-src: dist-all dist-src-extra


dist-bin:
	@echo \"Making custom binary targets...\"
	@cd \$(top_builddir); \\
	list='\$(EXTRA_BIN_DISTS)'; \\
	for dist in \$\$list; do \\
	    \$(MAKE) \$(AM_MAKEFLAGS) dist-\$\$dist; \\
	done

all-dist dist2 dist-all2: dist-src dist-bin

all-dist-check dist2-check dist-all-check: dist-check dist-src-extra dist-bin
])
])# AX_EXTRA_DIST

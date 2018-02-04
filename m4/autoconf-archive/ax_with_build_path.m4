# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_with_build_path.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_WITH_BUILD_PATH
#
# DESCRIPTION
#
#   This macro adds a "--with-build-path" option to the configure script.
#   This configure option provides a convenient way to add "-I" options to
#   CPPFLAGS and "-L" options to LDFLAGS, at configure time.
#
#   Invoking "./configure --with-build-path=DIR" results in "-I DIR/include"
#   being added to CPPFLAGS if DIR/include exists, and "-L DIR/lib" being
#   added to LDFLAGS if DIR/lib exists.
#
#   Separate multiple directories using colons; e.g.
#   "--with-build-path=DIR1:DIR2:DIR3".
#
# LICENSE
#
#   Copyright (c) 2008 Steve M. Robbins <smr@debian.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

AU_ALIAS([SMR_WITH_BUILD_PATH], [AX_WITH_BUILD_PATH])
AC_DEFUN([AX_WITH_BUILD_PATH],
[
    AC_ARG_WITH([build-path],
    [  --with-build-path=DIR   build using DIR/include and DIR/lib],
    [
        for d in `echo $withval | tr : ' '`; do
            test -d $d/include && CPPFLAGS="$CPPFLAGS -I$d/include"
            test -d $d/lib && LDFLAGS="$LDFLAGS -L$d/lib"
        done
    ])
])

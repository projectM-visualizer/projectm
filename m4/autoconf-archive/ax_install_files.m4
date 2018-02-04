# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_install_files.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_INSTALL_FILES
#
# DESCRIPTION
#
#   Adds target for creating a install_files file, which contains the list
#   of files that will be installed.
#
# LICENSE
#
#   Copyright (c) 2009 Tom Howard <tomhoward@users.sf.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 14

AC_DEFUN([AX_INSTALL_FILES],
[
AC_MSG_NOTICE([adding install_files support])
AC_ARG_VAR(GAWK, [gawk executable to use])
if test "x$GAWK" = "x"; then
   AC_CHECK_PROGS(GAWK,[gawk])
fi
AC_SUBST(GAWK)

if test "x$GAWK" != "x"; then
   AC_MSG_NOTICE([install_files support enabled])
   AX_HAVE_INSTALL_FILES=true
else
    AX_HAVE_INSTALL_FILES=false;
    AC_MSG_WARN([install_files support disable... gawk not found])
fi
AM_CONDITIONAL([ax_install_files_enabled],
               [test "x$AX_HAVE_INSTALL_FILES" = "xtrue"])

AX_ADD_AM_MACRO_STATIC([

if ax_install_files_enabled
AX_INSTALL_FILES_CLEANFILES = \\
\$(top_builddir)/install_files

\$(top_builddir)/install_files: do-mfstamp-recursive
	@if test \"\$(top_builddir)/mfstamp\" -nt \"\$(top_builddir)/install_files\"; then \\
	cd \$(top_builddir) && STAGING=\"\$(PWD)/staging\"; \\
	\$(MAKE) \$(AM_MAKEFLAGS) DESTDIR=\"\$\$STAGING\" install; \\
	cd \"\$\$STAGING\" && find "." ! -type d -print | \\
	\$(GAWK) ' \\
	    /^\\.\\/usr\\/local\\/lib/ { \\
		sub( /\\.\\/usr\\/local\\/lib/, \"%%{_libdir}\" ); } \\
	    /^\\.\\/usr\\/local\\/bin/ { \\
		sub( /\\.\\/usr\\/local\\/bin/, \"%%{_bindir}\" ); } \\
	    /^\\.\\/usr\\/local\\/include/ { \\
		sub( /\\.\\/usr\\/local\\/include/, \"%%{_includedir}\" ); } \\
	    /^\\.\\/usr\\/local\\/share/ { \\
		sub( /\\.\\/usr\\/local\\/share/, \"%%{_datadir}\" ); } \\
	    /^\\.\\/usr\\/local/ { \\
		sub( /\\.\\/usr\\/local/, \"%%{_prefix}\" ); } \\
	    /^\\./ { sub( /\\./, \"\" ); } \\
	    /./ { print; }' > ../install_files; \\
	rm -rf \"\$\$STAGING\"; \\
	else \\
	    echo \"\\\`\$(top_builddir)/install_files\' is up to date.\"; \\
	fi

])
AX_ADD_RECURSIVE_AM_MACRO_STATIC([do-mfstamp],[
\$(top_builddir)/mfstamp:  do-mfstamp-recursive

do-mfstamp-am do-mfstamp: Makefile.in
	@echo \"timestamp for all Makefile.in files\" > \$(top_builddir)/mfstamp
	@touch ${AX_DOLLAR}@

])
AX_ADD_AM_MACRO_STATIC([
endif # ax_install_files_enabled

clean-local: clean-ax-install-files
clean-ax-install-files:
	-test -z \"\$(AX_INSTALL_FILES)\" || rm -f \$(AX_INSTALL_FILES)
])
])# AX_INSTALL_FILES

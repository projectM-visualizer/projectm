# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_dist_rpm.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_DIST_RPM([SPEC])
#
# DESCRIPTION
#
#   Adds support for a rpm dist target.
#
#   You will need to create a spec template with everything except the files
#   and the Changelog. Use @PACKAGE@ and @VERSION@ to refer to the package
#   name and version respectively.  The files and ChangeLog will be filled
#   in automatically. For instance:
#
#     Summary: Foobar
#     Name: @PACKAGE@
#     Version: @VERSION@
#     Release: 0
#     License: GPL
#     Group: Productivity/Networking
#     Source0: http://somewhere/Foobar/%{name}-%{version}.tar.gz
#     URL: http://somewhere
#     BuildRoot: %{_tmppath}/%{name}-root
#     Prefix: %{_prefix}
#
#     %description
#     Foobar does something
#
#     %prep
#     %setup
#
#     %build
#     %configure
#     make
#
#     %install
#     %makeinstall
#
#     %clean
#     rm -rf $RPM_BUILD_ROOT
#
#     %files
#     %defattr(-,root,root)
#
#     %doc AUTHORS BUGS COPYING INSTALL NEWS README
#
#     %changelog
#
#   Make sure ax_upload.am is added to aminclude.am and you have 'include
#   aminclude.am' in your toplevel Makefile.am
#
# LICENSE
#
#   Copyright (c) 2009 Tom Howard <tomhoward@users.sf.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 22

AC_DEFUN([AX_DIST_RPM],
[
AC_REQUIRE([AX_INSTALL_FILES])
AC_MSG_NOTICE([adding rpm support])
AM_CONDITIONAL([ax_dist_rpm_enable_upload],[test "x$USING_AX_UPLOAD" != "x"])
AM_CONDITIONAL([ax_dist_rpm_using_ax_extra_dist],[test "x$USING_AX_EXTRA_DIST" != "x"])
ax_dist_rpm_enabled=false
if test "x$AX_HAVE_INSTALL_FILES" = "xtrue"; then
    AC_MSG_CHECKING([rpm spec template])
    if test -f "$srcdir/$1.in"; then
        AC_MSG_RESULT([$1.in])
        AX_RPM_SPEC_FILE="$1";
	AC_SUBST(AX_RPM_SPEC_FILE)
	AC_CONFIG_FILES([$1])
	AC_ARG_VAR(EDITOR, [default text editor])
	if test "x$EDITOR" = "x"; then
	    AC_CHECK_PROGS(EDITOR,[vim vi emacs])
	fi
	AC_ARG_VAR(RPM, [rpm executable to use])
	if test "x$RPM" = "x"; then
	    AC_CHECK_PROGS(RPM,[rpmbuild rpm echo])
	fi
	if test "x$RPM" != "x"; then
	    AC_ARG_VAR(PLATFORM_SUFFIX, [A short name for your platform that will be added to the name of the binary RPMs you may choose to create (by running make rpm).  e.g. rh71 for RedHat 7.1, lsb1.3 for a LSB 1.3 compliant system (SuSE 8.2 or RedHat 9), osx103 for OS X 10.3.x, etc])
	    AC_MSG_CHECKING([rpm platform suffix])
            if test "x$PLATFORM_SUFFIX" != "x"; then
		AC_MSG_RESULT([$PLATFORM_SUFFIX])
		AC_SUBST([PLATFORM_SUFFIX])
		ax_dist_rpm_enabled=true
            else
		AC_MSG_RESULT([$PLATFORM_SUFFIX])
		AC_MSG_WARN([rpm support disabled... PLATFORM_SUFFIX not set])
	    fi
        else
	    AC_MSG_NOTICE([rpm support disabled... neither rpmbuild or rpm was found])
	fi
    else
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([rpm spec template "$1.in" could not be found])
    fi
else
    AC_MSG_NOTICE([rpm support disabled... install_files not available])
fi
AM_CONDITIONAL([ax_dist_rpm_enabled], [test x"$ax_dist_rpm_enabled" = x"true"])

AX_ADD_AM_MACRO_STATIC([
if ax_dist_rpm_enabled

AX_DIST_RPM_CLEANFILES = \$(top_builddir)/RPMChangeLog \$(AX_INSTALL_FILES_CLEANFILES)

\$(top_builddir)/RPMChangeLog: \$(top_srcdir)/ChangeLog
	\$(GAWK) '/^[^0-9]/ { \\
                    if( \$${AX_DOLLAR}1 == \"*\" ) \$${AX_DOLLAR}1 = \"-\"; print; } \\
                /^\$\$/ { \\
                    print; } \\
                /^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]/ { \\
                    if( stop ) exit; \\
		    split( \$${AX_DOLLAR}1, date, \"-\" ); \\
		    \$${AX_DOLLAR}1 = strftime( \"%%a %%b %%d %%Y\", \\
				    mktime( date[ 1 ] \" \" date[ 2 ] \" \" date [ 3 ] \" 00 00 00\" ) ); \\
                    print \"* \" \$${AX_DOLLAR}0; } \\
		/Released \$(PACKAGE)-/ { \\
		    gsub( /\$(PACKAGE)-/, \"\", \$${AX_DOLLAR}2 ); \\
		    split( \$${AX_DOLLAR}2, ver, \".\" ); \\
		    split( \"\$(VERSION)\", curr_ver, \".\" ); \\
		    if( ver[ 1 ] != curr_ver[ 1 ] || ver[ 2 ] != curr_ver[ 2 ] ) stop = 1; } \\
		END { \\
		    print \"[Please see the ChangeLog file for older changes] - Ed.\"; }' \\
		\$(top_srcdir)/ChangeLog > \$(top_builddir)/RPMChangeLog

AX_DIST_RPM_CLEANFILES += \$(top_builddir)/\$(PACKAGE)-\$(VERSION).spec
AX_DIST_RPM_EXTRA_DIST = \$(top_builddir)/\$(PACKAGE)-\$(VERSION).spec \$(top_builddir)/\$(AX_RPM_SPEC_FILE).in

spec: \$(top_builddir)/\$(PACKAGE)-\$(VERSION).spec

\$(top_builddir)/\$(PACKAGE)-\$(VERSION).spec:	\$(top_builddir)/\$(AX_RPM_SPEC_FILE) \$(top_builddir)/install_files \$(top_builddir)/RPMChangeLog
	@cat \"\$(top_builddir)/\$(AX_RPM_SPEC_FILE)\" \\
	| \$(GAWK) -v files=\"\$\$files\" \\
	    '{ print; } \\
	    /%%defattr/ { while((getline < \"install_files\" ) > 0 ) { print; } }' \\
	    > \"\$(top_builddir)/\$(PACKAGE)-\$(VERSION).spec\"
	@cat \"\$(top_builddir)/RPMChangeLog\" >> \"\$(top_builddir)/\$(PACKAGE)-\$(VERSION).spec\"

AX_DIST_RPM_CLEANFILES += \\
\$(top_builddir)/*.rpm \\
\$(top_builddir)/rpmmacros

~/.rpmmacros:
	@echo \"~/.rpmmacros not found.  Creating one like the following:\"; \\
	echo \"\"; \\
	echo \"%%packager       <YOUR NAME>\"; \\
	echo \"\"; \\
	echo \"%%distribution   <YOUR_DISTRIBUTION>\"; \\
	echo \"%%vendor         <YOUR_ORGANISATION>\"; \\
	echo \"\"; \\
	echo \"%%_topdir        \$\$HOME/\"; \\
	echo \"%%_tmppath       /tmp/\"; \\
	echo \"\"; \\
	echo \"%%_rpmtopdir     %%{_topdir}rpm/\"; \\
	echo \"%%_builddir      %%{_tmppath}\"; \\
	echo \"%%_rpmdir        %%{_rpmtopdir}RPMS/\"; \\
	echo \"%%_sourcedir     %%{_rpmtopdir}SOURCES/\"; \\
	echo \"%%_specdir       %%{_rpmtopdir}SPECS/\"; \\
	echo \"%%_srcrpmdir     %%{_rpmtopdir}SRPMS/\"; \\
	echo \"\"; \\
	echo \"Where <YOUR_NAME> is your name and <YOUR_DISTRIBUTION> is the\"; \\
	echo \"distribution you are building on (e.g. SuSE Linux 8.2 or\"; \\
	echo \"RedHat Linux 7.1). and <YOUR_ORGANISATION> is the organisation\"; \\
	echo \"you are associated with.\"; \\
	echo \"\"; \\
	echo \"Running 'make create-rpmmacros' will create a ~/.rpmmacros file\"; \\
	echo \"for you and try to open it in an editor\"; \\
	exit -1

create-rpmmacros:
	@if test -e ~/.rpmmacros; then \\
	    echo \"Error: ~/.rpmmacros already exists\"; \\
	    echo \"Please remove it if you want to use\"; \\
	    echo \"this command to replace it\"; \\
	    exit -1; \\
	else \\
	    echo \"%%packager       <YOUR NAME>\" > ~/.rpmmacros; \\
	    echo \"\" >> ~/.rpmmacros; \\
	    echo \"%%distribution   <YOUR_DISTRIBUTION>\" >> ~/.rpmmacros; \\
	    echo \"%%vendor         <YOUR_ORGANISATION>\" >> ~/.rpmmacros; \\
	    echo \"\" >> ~/.rpmmacros; \\
	    echo \"%%_topdir        \$\$HOME/\" >> ~/.rpmmacros; \\
	    echo \"%%_tmppath       /tmp/\" >> ~/.rpmmacros; \\
	    echo \"\" >> ~/.rpmmacros; \\
	    echo \"%%_rpmtopdir     %%{_topdir}rpm/\" >> ~/.rpmmacros; \\
	    echo \"%%_builddir      %%{_tmppath}\" >> ~/.rpmmacros; \\
	    echo \"%%_rpmdir        %%{_rpmtopdir}RPMS/\" >> ~/.rpmmacros; \\
	    echo \"%%_sourcedir     %%{_rpmtopdir}SOURCES/\" >> ~/.rpmmacros; \\
	    echo \"%%_specdir       %%{_rpmtopdir}SPECS/\" >> ~/.rpmmacros; \\
	    echo \"%%_srcrpmdir     %%{_rpmtopdir}SRPMS/\" >> ~/.rpmmacros; \\
	    if test \"x$EDITOR\" = "x"; then \\
		echo \"Error: no editor specified or found.\"; \\
		echo \"Please edit ~/.rpmmacros manually\"; \\
	    else \\
		$EDITOR ~/.rpmmacros; \\
		exit \$\$?; \\
	    fi \\
	fi

rpmmacros: ~/.rpmmacros
	@cp -f ~/.rpmmacros rpmmacros;

dist-rpm: rpm
dist-srpm: srpm

rpm: \$(PACKAGE)-\$(VERSION)-0.i*.$PLATFORM_SUFFIX.rpm
srpm: \$(PACKAGE)-\$(VERSION)-0.src.rpm

\$(top_builddir)/\$(PACKAGE)-\$(VERSION)-0.i*.$PLATFORM_SUFFIX.rpm:	\$(top_builddir)/rpmmacros \$(top_builddir)/\$(PACKAGE)-\$(VERSION).tar.gz
	@\$(RPM) -tb \$(top_builddir)/\$(PACKAGE)-\$(VERSION).tar.gz
	@RPMDIR=\`cat \$(top_builddir)/rpmmacros | \$(GAWK) '/%%_rpmdir/ { print \$${AX_DOLLAR}2; }'\`; \\
	echo \"\$\$RPMDIR\" | \$(EGREP) \"%%{.*}\" > /dev/null 2>&1; \\
	EXIT=\$\$?; \\
	while test \"\$\$EXIT\" == \"0\"; do \\
		RPMDIR=\`echo \"\$\$RPMDIR\" | \$(GAWK) '/%%{.*}/ \\
		{ match( \$${AX_DOLLAR}0, /%%{.*}/, macro ); \\
		  suffix = substr( \$${AX_DOLLAR}0, RSTART + RLENGTH ); \\
		  gsub( /{|}/, \"\", macro[ 0 ] ); \\
		  while( ( getline < \"\$(top_builddir)/rpmmacros\" ) > 0 ) \\
		  { if( \$${AX_DOLLAR}1 == macro[ 0 ] ) { print \$${AX_DOLLAR}2 suffix; exit; } } \\
		  exit 1; \\
		}'\`; \\
		if test \"\$${AX_DOLLAR}?\" == \"0\"; then \\
			echo \"\$\$RPMDIR\" | \$(EGREP) \"%%{.*}\" > /dev/null 2>&1; \\
			EXIT=\"\$${AX_DOLLAR}?\"; \\
		else \\
			EXIT=\"1\"; \\
		fi; \\
	done; \\
	if test -d \"\$\$RPMDIR\"; then \\
	    for dir in \`ls \"\$\$RPMDIR\"\`; do \\
		ls \"\$\${RPMDIR}\$\${dir}/\$(PACKAGE)-\$(VERSION)-0.\$\${dir}.rpm\" > /dev/null 2>&1; \\
		if test \"\$${AX_DOLLAR}?\" == \"0\"; then \\
		    cp \"\$\${RPMDIR}\$\${dir}/\$(PACKAGE)-\$(VERSION)-0.\$\${dir}.rpm\" \"\$(top_builddir)/\$(PACKAGE)-\$(VERSION)-0.\$\${dir}.$PLATFORM_SUFFIX.rpm\"; \\
		    found=true; \\
		fi; \\
	    done; \\
	    if ! \$\$found; then \\
		echo \"RPM built but not found.\"; \\
		echo \"Please copy it to \$(top_builddir) manually.\"; \\
		exit 1; \\
	    fi; \\
	else \\
	    echo \"RPM built but I cannot find RPM directory.\"; \\
	    echo \"Please copy it to \$(top_builddir) manually.\"; \\
	    exit 1; \\
	fi;

\$(top_builddir)/\$(PACKAGE)-\$(VERSION).tar.gz: \$(top_builddir)/install_files
	cd \"\$(top_builddir)\" && \$(MAKE) \$(AM_MAKEFLAGS) dist-gzip




\$(PACKAGE)-\$(VERSION)-0.src.rpm:	\$(top_builddir)/rpmmacros \$(top_builddir)/\$(PACKAGE)-\$(VERSION).tar.gz
	@\$(RPM) -ts \$(top_builddir)/\$(PACKAGE)-\$(VERSION).tar.gz
	@SRPMDIR=\`cat rpmmacros | \$(GAWK) '/%%_srcrpmdir/ { print ${AX_DOLLAR}${AX_DOLLAR}2; }'\`; \\
	echo \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\" | \$(EGREP) \"%%{.*}\" > /dev/null 2>&1; \\
	EXIT=${AX_DOLLAR}${AX_DOLLAR}?; \\
	while test \"${AX_DOLLAR}${AX_DOLLAR}EXIT\" == \"0\"; do \\
	    SRPMDIR=\`echo \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\" | \$(GAWK) '/%%{.*}/ \\
		{ match( ${AX_DOLLAR}${AX_DOLLAR}0, /%%{.*}/, macro ); \\
		  suffix = substr( ${AX_DOLLAR}${AX_DOLLAR}0, RSTART + RLENGTH ); \\
		  gsub( /{|}/, \"\", macro[ 0 ] ); \\
		  while( ( getline < \"rpmmacros\" ) > 0 ) \\
		  { if( ${AX_DOLLAR}${AX_DOLLAR}1 == macro[ 0 ] ) { print ${AX_DOLLAR}${AX_DOLLAR}2 suffix; exit; } } \\
		  exit 1; \\
		}'\`; \\
		if test \"${AX_DOLLAR}${AX_DOLLAR}?\" == \"0\"; then \\
			echo \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\" | \$(EGREP) \"%%{.*}\" > /dev/null 2>&1; \\
			EXIT=\"${AX_DOLLAR}${AX_DOLLAR}?\"; \\
		else \\
			EXIT=\"1\"; \\
		fi; \\
	done; \\
	if test -d \"${AX_DOLLAR}${AX_DOLLAR}SRPMDIR\"; then \\
	    ls \"${AX_DOLLAR}${AX_DOLLAR}{SRPMDIR}/\$(PACKAGE)-\$(VERSION)-0.src.rpm\" > /dev/null 2>&1; \\
	    if test \"${AX_DOLLAR}${AX_DOLLAR}?\" == \"0\"; then \\
		cp \"\$${AX_DOLLAR}{SRPMDIR}/\$(PACKAGE)-\$(VERSION)-0.src.rpm\" \"${AX_DOLLAR}(top_builddir)/.\"; \\
		found=true; \\
	    fi; \\
	    if ! ${AX_DOLLAR}${AX_DOLLAR}found; then \\
		echo \"SRPM built but not found.\"; \\
		echo \"Please copy it to ${AX_DOLLAR}(top_builddir) manually.\"; \\
		exit 1; \\
	    fi; \\
	else \\
	     echo \"SRPM built but I cannot find SRPM directory.\"; \\
	     echo \"Please copy it to ${AX_DOLLAR}(top_builddir) directory manually.\"; \\
	     exit 1; \\
	fi;


AX_DIST_RPM_EXTRA_BIN_DISTS = rpm
AX_DIST_RPM_EXTRA_SRC_DISTS = srpm

if ax_dist_rpm_enable_upload
AX_DIST_RPM_UPLOAD_BIN = upload-rpm
AX_DIST_RPM_UPLOAD_SRC = upload-srpm

AX_DIST_RPM_UPLOAD_TARGETS = \\
{rpm=>$PACKAGE-$VERSION-0.i*.$PLATFORM_SUFFIX.rpm} \\
{srpm=>$PACKAGE-$VERSION-0.src.rpm}
endif # ax_dist_rpm_enable_upload

endif # ax_dist_rpm_enabled

# These rules are included unconditionally since we cannot conditionally add
# dependencies.
clean-local: clean-ax-dist-rpm
clean-ax-dist-rpm:
	-test -z \"\$(AX_DIST_RPM_CLEANFILES)\" || rm -f \$(AX_DIST_RPM_CLEANFILES)

dist-hook: dist-hook-ax-dist-rpm
dist-hook-ax-dist-rpm: \$(AX_DIST_RPM_EXTRA_DIST)
	-test -z \"\$(AX_DIST_RPM_EXTRA_DIST)\" || cp \$(AX_DIST_RPM_EXTRA_DIST) \$(distdir)

])
])

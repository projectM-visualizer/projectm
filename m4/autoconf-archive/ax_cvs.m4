# ===========================================================================
#          https://www.gnu.org/software/autoconf-archive/ax_cvs.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CVS([ANON_CVSROOT])
#
# DESCRIPTION
#
#   Adds support cvs targets within your Makefile.
#
#   Branching and releasing relies on you using the following version
#   format:
#
#     MAJOR.MINOR.POINT
#
#   where MAJOR is the major version number, MINOR is the minor version
#   number and POINT is the point release number.
#
#   make update
#
#     performs a cvs update
#
#   make commitlog
#
#     Generates a ChangeLog template with the modified, added
#     and removed files and opens it up for editing. This is
#     not normally used directly, but is instead called by
#     commit. It is useful however if you want to document
#     the work you have done, but don't want to commit just
#     yet.
#
#   make commit
#
#     performs a cvs commit after first performing a cvs
#     update and generating a commit log.
#
#   make cvs-rm FILES="file1 file2 ..."
#
#     removes a file or files specified by the FILES
#     variable from the file system and from cvs. It will
#     interactively ask you to confirm the file removal,
#     unless the file does not exist on the file system.
#
#   make cvs-add FILES="file1 file2 ..."
#
#     adds a file or files specified by the FILES variable
#     to cvs.
#
#   make branch-major
#
#     creates a branch based on the major version number,
#     increments the major version number, sets the minor
#     and point versions to zero and checks out the branch
#     into a new directory.
#
#   make branch-minor
#
#     creates a branch based on the minor version number,
#     increments the minor version, sets the point version
#     to zero number and checks out the branch into a new
#     directory.
#
#   make release
#
#     performs a cvs update, followed by a distcheck, then
#     creates a release for the current version, increments
#     the point release number and checks out the release
#     into a new directory
#
#   make quick-release
#
#     same as release, but distcheck is not performed
#
# LICENSE
#
#   Copyright (c) 2009 Tom Howard <tomhoward@users.sf.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 21

AC_DEFUN([AX_CVS],
[
AC_REQUIRE([AX_SPLIT_VERSION])

AC_SUBST([ANON_CVSROOT], [$1])
AC_MSG_CHECKING([ANON_CVSROOT])
if test "x$ANON_CVSROOT" != "x"; then
    AC_MSG_RESULT([$ANON_CVSROOT])
else
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([An anonymous CVSROOT must be specified to enable CVS support])
fi


AC_ARG_ENABLE(cvs-support,
	      AS_HELP_STRING(--enable-cvs-support[=ARG],
			     [enable cvs support.  Used by the $PACKAGE developers.
			      ARG can be "yes" or "no".  If the CVSROOT is found,
			      then the default is yes,
			      otherwise the default is no.]),
	      AX_USING_CVS=$enableval )

if test "x$AX_USING_CVS" != "xno"; then
    AC_ARG_VAR(CVS, [cvs executable to use])
    if test "x$CVS" = "x"; then
        AC_CHECK_PROGS(CVS,[cvs])
    fi
    if test "x$CVS" = "x"; then
        if test "x$AX_USING_CVS" = "x"; then
	    AX_USING_CVS=no
	else
	    AC_MSG_ERROR([CVS support cannot be enabled: cvs executable not found])
	fi
    fi
    AC_SUBST([CVS])
fi

if test "x$AX_USING_CVS" != "xno"; then
   AC_ARG_VAR(GAWK, [gawk executable to use])
   if test "x$GAWK" = "x"; then
      AC_CHECK_PROGS(GAWK,[gawk])
   fi
   if test "x$GAWK" = "x"; then
       if test "x$AX_USING_CVS" = "x"; then
	    AX_USING_CVS=no
	else
	    AC_MSG_ERROR([CVS support cannot be enabled: gawk could not be found])
	fi
    fi
    AC_SUBST([GAWK])
fi

if test "x$AX_USING_CVS" != "xno"; then
    AC_ARG_VAR(CVSEDITOR, [text editor to use for cvs])
    if test "x$CVSEDITOR" = "x"; then
        if test "x$EDITOR" = "x"; then
           AC_CHECK_PROGS(CVSEDITOR,[vim vi emacs])
	else
           AC_CHECK_PROGS(CVSEDITOR,[$EDITOR vim vi emacs])
	fi
    fi
    if test "x$CVSEDITOR" = "x"; then
        if test "x$AX_USING_CVS" = "x"; then
	    AX_USING_CVS=no
	else
	    AC_MSG_ERROR([CVS support cannot be enabled: CVSEDITOR not set and editor not found])
	fi
    fi
    AC_SUBST([CVSEDITOR])
fi

if test "x$AX_USING_CVS" != "xno"; then
    AC_ARG_VAR(CVSROOT, [the CVSROOT to use])
    AC_MSG_CHECKING([CVSROOT])
    if test -e "$srcdir/CVS/Root"; then
        CVSROOT=`cat $srcdir/CVS/Root`;
	AC_MSG_RESULT([$CVSROOT])
    else
	AC_MSG_RESULT([not found])
        if test "x$AX_USING_CVS" = "x"; then
	    AX_USING_CVS=no
	else
	    AC_MSG_ERROR([CVS support cannot be enabled: CVSROOT not found.  Did you check out from CVS?])
	fi
    fi
    AC_SUBST([CVSROOT])
fi

if test "x$AX_USING_CVS" != "xno"; then
    AC_MSG_CHECKING([USERNAME])
    if test "x$USERNAME" != "x"; then
        AC_MSG_RESULT([$USERNAME])
    else
        AC_MSG_RESULT([not set])
        if test "x$AX_USING_CVS" = "x"; then
	    AX_USING_CVS=no
	else
            AC_MSG_ERROR([CVS support cannot be enabled: USERNAME environment variable not set])
	fi
    fi
    AC_SUBST([USERNAME])
fi

if test "x$AX_USING_CVS" != "xno"; then
    AC_MSG_CHECKING([USEREMAIL])
    if test "x$USERNAME" != "x"; then
        AC_MSG_RESULT([$USEREMAIL])
    else
        AC_MSG_RESULT([not set])
        if test "x$AX_USING_CVS" = "x"; then
	    AX_USING_CVS=no
	else
            AC_MSG_ERROR([CVS support cannot be enabled: USEREMAIL environment variable not set])
	fi
    fi
    AC_SUBST([USEREMAIL])
fi


if test "x$AX_USING_CVS" != "xno"; then
   AC_MSG_NOTICE([cvs support enabled])
   m4_pattern_allow([AC_INIT])
   AX_PRINT_TO_FILE([ax_cvs_rel.awk],[[

BEGIN {
    if( change <= 0 )
        exit -1;
}

/AC_INIT/ {
    if( ${AX_DOLLAR}2 ~ /\\)${AX_DOLLAR}/ )
        {
            ver = substr( ${AX_DOLLAR}2, 0, length( ${AX_DOLLAR}2 ) - 1 );
            tail=\")\";
        }
    else
        ver = ${AX_DOLLAR}2;

    if( ${AX_DOLLAR}2 ~ /${AX_BS}${AX_SRB}${AX_DOLLAR}/ )
        {
            ver = substr( ${AX_DOLLAR}2, 0, length( ${AX_DOLLAR}2 ) - 1 );
            tail=\"${AX_SRB}\";
        }
    else
        ver = ${AX_DOLLAR}2;

    if( ${AX_DOLLAR}2 ~ /\\,${AX_DOLLAR}/ )
        {
            ver = substr( ${AX_DOLLAR}2, 0, length( ${AX_DOLLAR}2 ) - 1 );
            tail=\",\";
        }
    else
        ver = ${AX_DOLLAR}2;

    n = split( ver, ver_array, \".\" );

    while( change > n )
        ver_array[ ++n ] = 0;

    ver_array[ change ]++;

    while( ++change <= n )
        ver_array[ change ] = 0;

    ${AX_DOLLAR}2 = \"\";
    for( i = 1; i < n; ++i )
        ${AX_DOLLAR}2 = ${AX_DOLLAR}2 ver_array[ i ] \".\";
    ${AX_DOLLAR}2 = ${AX_DOLLAR}2 ver_array[ n ];
    if( tail )
        ${AX_DOLLAR}2 = ${AX_DOLLAR}2 tail;
}

{ print ${AX_DOLLAR}0; }

]])
fi
AM_CONDITIONAL([ax_cvs_enabled],[test "x$AX_USING_CVS" != "xno"])
AX_ADD_AM_MACRO_STATIC([
if ax_cvs_enabled
update:
	@cd \"${AX_DOLLAR}(srcdir)\" && \$(CVS) -z9 update

cvsalways:

${AX_DOLLAR}(top_builddir)/commitlog: cvsalways
	@(CURR=\`pwd\`; cd \"${AX_DOLLAR}(top_srcdir)\"; \$(CVS) -z9 diff -u --brief 2>&1 | \\
	\$(GAWK) \\
	\'/^Index/ { print \"\\\\t* ./\" ${AX_DOLLAR}${AX_DOLLAR}2; } \\
	/^cvs diff: .* was removed/ { print \"\\\\t* ./\" ${AX_DOLLAR}${AX_DOLLAR}3 \" (removed)\"; } \\
	/^cvs diff: .* is a new entry/ { print \"\\\\t* ./\" ${AX_DOLLAR}${AX_DOLLAR}3 \" (added)\"; }\' \\
	> \"${AX_DOLLAR}${AX_DOLLAR}CURR/commitlog.tmp\")
	@if test -s commitlog.tmp; then \\
		echo \"\" >> commitlog.tmp; \\
		if test -f ${AX_DOLLAR}(top_builddir)/commitlog; then \\
			cat ${AX_DOLLAR}(top_builddir)/commitlog >> commitlog.tmp; \\
		fi; \\
		echo \"/* -*-change-log-*- */\" > ${AX_DOLLAR}(top_builddir)/commitlog; \\
		DATE=\`date +\"%%Y-%%m-%%d\"\`; \\
		echo \"${AX_DOLLAR}${AX_DOLLAR}DATE	${USERNAME}	<${USEREMAIL}>\" >> ${AX_DOLLAR}(top_builddir)/commitlog; \\
		echo \"\" >> ${AX_DOLLAR}(top_builddir)/commitlog; \\
		cat commitlog.tmp  >> ${AX_DOLLAR}(top_builddir)/commitlog; \\
		rm -f commitlog.tmp; \\
		\$(CVSEDITOR) ${AX_DOLLAR}(top_builddir)/commitlog; \\
		\$(GAWK) \'BEGIN { blank=0; } \\
		/\\\\/\\\\* -\\\\*-change-log-\\\\*- \\\\*\\\\// { getline; } \\
		/^[[:blank:]]*\$\$/ { if( !blank ) { blank = 1; print; } } \\
		/[[:alnum:]]/ { print; blank = 0; } \\
		END{ if( !blank ) print \"\"; }\' \\
		${AX_DOLLAR}(top_builddir)/commitlog > commitlog.tmp; \\
		mv commitlog.tmp ${AX_DOLLAR}(top_builddir)/commitlog; \\
	else \\
		echo \"no changes found\";\\
	fi

commit: update ${AX_DOLLAR}(top_builddir)/commitlog
	@if test -f ${AX_DOLLAR}(top_builddir)/commitlog; then \\
	    cat ${AX_DOLLAR}(top_builddir)/commitlog \"\$(top_srcdir)/ChangeLog\" > ChangeLog.tmp; \\
	    mv ChangeLog.tmp \"\$(top_srcdir)/ChangeLog\"; \\
	    CURR=\`(cd \"${AX_DOLLAR}(top_builddir)\"; pwd )\`; \\
	    (cd \"\$(top_srcdir)\"; \$(CVS) -z9 commit -F \"${AX_DOLLAR}${AX_DOLLAR}CURR/commitlog\" ); \\
	    rm -f ${AX_DOLLAR}(top_builddir)/commitlog; \\
	fi

cvs-rm:
	@echo \"Removing files from CVS\"
	@if test \"x\$(FILES)\" != \"x\"; then \\
	    NEWFILES=\"\"; \\
	    for FILE in \$(FILES); do \\
		if test -e \"${AX_DOLLAR}${AX_DOLLAR}FILE\"; then \\
		    rm -i \"${AX_DOLLAR}${AX_DOLLAR}FILE\"; \\
		fi; \\
		if test ! -e \"${AX_DOLLAR}${AX_DOLLAR}FILE\"; then \\
		    if test \"\$(srcdir)\" != \".\"; then \\
			FILE=\`echo \"${AX_DOLLAR}${AX_DOLLAR}FILE\" | \$(SED) -e \'s|^\$(srcdir)|.|\'\`; \\
		    fi; \\
		    NEWFILES=\"${AX_DOLLAR}${AX_DOLLAR}NEWFILES ${AX_DOLLAR}${AX_DOLLAR}FILE\"; \\
		fi; \\
	    done; \\
	    if test \"x${AX_DOLLAR}${AX_DOLLAR}NEWFILES\" != \"x\"; then \\
		(cd \"\$(srddir)\"; \$(CVS) remove ${AX_DOLLAR}${AX_DOLLAR}NEWFILES; ); \\
	    fi; \\
	else \\
	    echo \"You must specify the file(s) you want to remove from cvs by using\"; \\
	    echo \"the FILES environment variable.  For example:\"; \\
	    echo \"    make cvs-rm FILES=\'foo bar\'\"; \\
	    echo \"\"; \\
	    exit 1; \\
	fi

cvs-add:
	@echo \"Adding files to CVS\"
	@if test \"x${AX_DOLLAR}(FILES)\" != \"x\"; then \\
	    NEWFILES=\"\"; \\
	    for FILE in ${AX_DOLLAR}(FILES); do \\
		if test -e \"${AX_DOLLAR}${AX_DOLLAR}FILE\"; then \\
		    if test \"${AX_DOLLAR}(srcdir)\" != \".\"; then \\
			FILE=\`echo \"${AX_DOLLAR}${AX_DOLLAR}FILE\" | sed -e \'s|^${AX_DOLLAR}(srcdir)|.|\'\`; \\
		    fi; \\
		fi; \\
		NEWFILES=\"${AX_DOLLAR}${AX_DOLLAR}NEWFILES ${AX_DOLLAR}${AX_DOLLAR}FILE\"; \\
	    done; \\
	    if test \"x${AX_DOLLAR}${AX_DOLLAR}NEWFILES\" != \"x\"; then \\
		cd \"${AX_DOLLAR}(srcdir)\"; \$(CVS) add ${AX_DOLLAR}${AX_DOLLAR}NEWFILES; \\
	    fi; \\
	else \\
	    echo \"You must specify the file(s) you want to add to cvs by using\"; \\
	    echo \"the FILES environment variable.  For example:\"; \\
	    echo \"    make cvs-add FILES=\'foo bar\'\"; \\
	    echo \"\"; \\
	    exit 1; \\
	fi

branch-major:
	@tag=\"\$(PACKAGE)-${AX_MAJOR_VERSION}\"; \\
	echo \"\"; \\
	echo \"Creating major brach: ${AX_DOLLAR}${AX_DOLLAR}tag\"; \\
	(cd \"${AX_DOLLAR}(top_srcdir)\"; \$(CVS) tag -b \"${AX_DOLLAR}${AX_DOLLAR}tag\"; ); \\
	\$(GAWK) -f ax_cvs_rel.awk -v change=1 \"${AX_DOLLAR}(top_srcdir)/configure.ac\" > configure.tmp; \\
	touch ${AX_DOLLAR}(top_builddir)/commitlog; \\
	DATE=\`date +\"%%Y-%%m-%%d\"\`; \\
	echo \"${AX_DOLLAR}${AX_DOLLAR}DATE	\$(USERNAME)	<\$(USEREMAIL)>\" > commitlog.tmp ; \\
	echo \"\" >> commitlog.tmp; \\
	echo \"	* ./configure.ac\" >> commitlog.tmp; \\
	echo \"	Created major branch: ${AX_DOLLAR}${AX_DOLLAR}tag\" >> commitlog.tmp; \\
	echo \"	Use:\" >> commitlog.tmp; \\
	echo \"	\\\\\`cvs -d\$(ANON_CVSROOT) login\\\\\`\" >> commitlog.tmp; \\
	echo \"	\\\\\`cvs -d\$(ANON_CVSROOT) co -r ${AX_DOLLAR}${AX_DOLLAR}tag \$(PACKAGE)\\\\\`\" >> commitlog.tmp; \\
	echo \"	to access the branch\" >> commitlog.tmp; \\
	echo \"\" >> commitlog.tmp; \\
	cat ${AX_DOLLAR}(top_builddir)/commitlog >> commitlog.tmp; \\
	mv commitlog.tmp ${AX_DOLLAR}(top_builddir)/commitlog; \\
	cat ${AX_DOLLAR}(top_builddir)/commitlog \"${AX_DOLLAR}(top_srcdir)/ChangeLog\" > ChangeLog.tmp; \\
	mv ChangeLog.tmp \"${AX_DOLLAR}(top_srcdir)/ChangeLog\"; \\
	mv configure.tmp \"${AX_DOLLAR}(top_srcdir)/configure.ac\"; \\
	CURR=\`(cd \"${AX_DOLLAR}(top_builddir)\"; pwd )\`; \\
	(cd \"${AX_DOLLAR}(top_srcdir)\"; \$(CVS) -z9 commit -F \"${AX_DOLLAR}${AX_DOLLAR}CURR/commitlog\"; ); \\
	rm -f ${AX_DOLLAR}(top_builddir)/commitlog; \\
	\$(CVS) -z9 -d${AX_DOLLAR}(CVSROOT) co -r ${AX_DOLLAR}${AX_DOLLAR}tag -d ${AX_DOLLAR}${AX_DOLLAR}tag \$(PACKAGE); \\
	echo \"The branch is now available in the ${AX_DOLLAR}${AX_DOLLAR}tag directory\"; \\
	echo \"\"


branch-minor:
	@tag=\"\$(PACKAGE)-${AX_MAJOR_VERSION}_${AX_MINOR_VERSION}\"; \\
	echo \"\"; \\
	echo \"Creating minor brach: ${AX_DOLLAR}${AX_DOLLAR}tag\"; \\
	(cd \"${AX_DOLLAR}(top_srcdir)\"; \$(CVS) tag -b \"${AX_DOLLAR}${AX_DOLLAR}tag\"; ); \\
	\$(GAWK) -f ax_cvs_rel.awk -v change=1 \"${AX_DOLLAR}(top_srcdir)/configure.ac\" > configure.tmp; \\
	touch ${AX_DOLLAR}(top_builddir)/commitlog; \\
	DATE=\`date +\"%%Y-%%m-%%d\"\`; \\
	echo \"${AX_DOLLAR}${AX_DOLLAR}DATE	\$(USERNAME)	<\$(USEREMAIL)>\" > commitlog.tmp ; \\
	echo \"\" >> commitlog.tmp; \\
	echo \"	* ./configure.ac\" >> commitlog.tmp; \\
	echo \"	Created minor branch: ${AX_DOLLAR}${AX_DOLLAR}tag\" >> commitlog.tmp; \\
	echo \"	Use:\" >> commitlog.tmp; \\
	echo \"	\\\\\`cvs -d\$(ANON_CVSROOT) login\\\\\`\" >> commitlog.tmp; \\
	echo \"	\\\\\`cvs -d\$(ANON_CVSROOT) co -r ${AX_DOLLAR}${AX_DOLLAR}tag \$(PACKAGE)\\\\\`\" >> commitlog.tmp; \\
	echo \"	to access the branch\" >> commitlog.tmp; \\
	echo \"\" >> commitlog.tmp; \\
	cat ${AX_DOLLAR}(top_builddir)/commitlog >> commitlog.tmp; \\
	mv commitlog.tmp ${AX_DOLLAR}(top_builddir)/commitlog; \\
	cat ${AX_DOLLAR}(top_builddir)/commitlog \"${AX_DOLLAR}(top_srcdir)/ChangeLog\" > ChangeLog.tmp; \\
	mv ChangeLog.tmp \"${AX_DOLLAR}(top_srcdir)/ChangeLog\"; \\
	mv configure.tmp \"${AX_DOLLAR}(top_srcdir)/configure.ac\"; \\
	CURR=\`(cd \"${AX_DOLLAR}(top_builddir)\"; pwd )\`; \\
	(cd \"${AX_DOLLAR}(top_srcdir)\"; \$(CVS) -z9 commit -F \"${AX_DOLLAR}${AX_DOLLAR}CURR/commitlog\"; ); \\
	rm -f ${AX_DOLLAR}(top_builddir)/commitlog; \\
	\$(CVS) -z9 -d${AX_DOLLAR}(CVSROOT) co -r ${AX_DOLLAR}${AX_DOLLAR}tag -d ${AX_DOLLAR}${AX_DOLLAR}tag \$(PACKAGE); \\
	echo \"The branch is now available in the ${AX_DOLLAR}${AX_DOLLAR}tag directory\"; \\
	echo \"\"

add_rel:
	@touch \"${AX_DOLLAR}(top_builddir)/commitlog\"
	@DATE=\`date +\"%%Y-%%m-%%d\"\`; \\
	echo \"${AX_DOLLAR}${AX_DOLLAR}DATE	\$(USERNAME)	<\$(USEREMAIL)>\" > commitlog.tmp
	@echo \"\" >> commitlog.tmp
	@echo \"	* ./configure.ac\" >> commitlog.tmp
	@echo \"	Released \$(PACKAGE)-\$(VERSION)\" >> commitlog.tmp
	@echo \"	You can access this release by running:\" >> commitlog.tmp
	@echo \"	    \\\\\`cvs -d\$(ANON_CVSROOT) login\\\\\`\" >> commitlog.tmp
	@tag=\"\$(PACKAGE)-${AX_MAJOR_VERSION}_${AX_MINOR_VERSION}_${AX_POINT_VERSION}\"; \\
	echo \"	    \\\\\`cvs -d\$(ANON_CVSROOT) co -r ${AX_DOLLAR}${AX_DOLLAR}tag -d \$(PACKAGE)-\$(VERSION) \$(PACKAGE)\\\\\`\" >> commitlog.tmp
	@echo \"	The release will then be available in the \$(PACKAGE)-\$(VERSION) directory\" >> commitlog.tmp
	@echo \"\" >> commitlog.tmp
	@cat \"${AX_DOLLAR}(top_builddir)/commitlog\" >> commitlog.tmp
	@mv commitlog.tmp \"${AX_DOLLAR}(top_builddir)/commitlog\"
	@cat \"${AX_DOLLAR}(top_builddir)/commitlog\" \"\$(top_srcdir)/ChangeLog\" > ChangeLog.tmp
	@mv ChangeLog.tmp \"\$(top_srcdir)/ChangeLog\"
	@CURR=\`(cd \"${AX_DOLLAR}(top_builddir)\"; pwd )\`; \\
	(cd \"\$(top_srcdir)\"; \$(CVS) -z9 commit -F \"${AX_DOLLAR}${AX_DOLLAR}CURR/commitlog\"; )
	@rm -f ${AX_DOLLAR}(top_builddir)/commitlog

do_tag:
	@tag=\"\$(PACKAGE)-${AX_MAJOR_VERSION}_${AX_MINOR_VERSION}_${AX_POINT_VERSION}\"; \\
	echo \"tagging release with ${AX_DOLLAR}${AX_DOLLAR}tag\"; \\
	(cd \"\$(top_srcdir)\"; \$(CVS) tag -b \"${AX_DOLLAR}${AX_DOLLAR}tag\"; ); \\
	\$(CVS) -z9 -d\$(CVSROOT) co -r ${AX_DOLLAR}${AX_DOLLAR}tag -d ${AX_DOLLAR}${AX_DOLLAR}tag \$(PACKAGE); \\
	echo \"The release is now available in the ${AX_DOLLAR}${AX_DOLLAR}tag directory\"; \\
	echo \"\"

inc_rel:
	@\$(GAWK) -f ax_cvs_rel.awk -v change=3 \"\$(top_srcdir)/configure.ac\" > configure.tmp;
	@mv configure.tmp \"\$(top_srcdir)/configure.ac\"
	@touch \"\$(top_builddir)/commitlog\"
	@DATE=\`date +\"%%Y-%%m-%%d\"\`; \\
	echo \"${AX_DOLLAR}${AX_DOLLAR}DATE	\$(USERNAME)	<\$(USEREMAIL)>\" > commitlog.tmp ; \\
	echo \"\" >> commitlog.tmp; \\
	echo \"	* ./configure.ac\" >> commitlog.tmp; \\
	echo \"	Update version number\" >> commitlog.tmp; \\
	echo \"\" >> commitlog.tmp; \\
	cat ${AX_DOLLAR}(top_builddir)/commitlog >> commitlog.tmp; \\
	mv commitlog.tmp ${AX_DOLLAR}(top_builddir)/commitlog; \\
	cat ${AX_DOLLAR}(top_builddir)/commitlog \"${AX_DOLLAR}(top_srcdir)/ChangeLog\" > ChangeLog.tmp; \\
	mv ChangeLog.tmp \"${AX_DOLLAR}(top_srcdir)/ChangeLog\"; \\
	CURR=\`(cd \"${AX_DOLLAR}(top_builddir)\"; pwd )\`; \\
	(cd \"${AX_DOLLAR}(top_srcdir)\"; \$(CVS) -z9 commit -F \"${AX_DOLLAR}${AX_DOLLAR}CURR/commitlog\"; ); \\
	rm -f ${AX_DOLLAR}(top_builddir)/commitlog;

tag: do_tag inc_rel

# creates a release for the current version, increments the point
# release number and checkout the release into a new directory
release: update distcheck add_rel tag

# same as release, but distcheck is not performed before releasing
quick-release:	update add_rel tag

endif # ax_cvs_enabled
])

])

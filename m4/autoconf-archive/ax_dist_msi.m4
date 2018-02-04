# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_dist_msi.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_DIST_MSI([File])
#
# DESCRIPTION
#
#   Adds support for a msi (Microsoft Installer) dist target.
#
#   You must manually build the msi file yourself (probably from another
#   computer). But it will be added to the list of extra bin dists and
#   flagged for uploading (see ax_extra_dist.m4 and ax_upload.m4 for
#   details).
#
# LICENSE
#
#   Copyright (c) 2009 Tom Howard <tomhoward@users.sf.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 15

AC_DEFUN([AX_DIST_MSI],
[
AC_REQUIRE([AX_INSTALL_FILES])
AC_MSG_NOTICE([adding dist-msi support])
MSI_SETUP_FILE="$1"
AC_SUBST(MSI_SETUP_FILE)
if test "x$MSI_SETUP_FILE" != "x"; then
    AC_MSG_NOTICE([setting msi file... $MSI_SETUP_FILE])
    AC_SUBST(USING_DIST_MSI)
    AC_ARG_ENABLE(dist-msi,
		  AS_HELP_STRING(--enable-dist-msi[=ARG],
				 [enable support for msi (Microsoft Installer)
				  dist target. ARG can be
                                  "yes" or "no".  The default is "yes"]),
		  if test "x$enableval" != "x"; then
		      if test "x$enableval" = "xyes"; then
			  USING_DIST_MSI=true
			  AC_MSG_NOTICE([dist-msi support enabled])
		      elif test "x$enableval" = "xno"; then
			  USING_DIST_MSI=false
			  AC_MSG_NOTICE([dist-msi support disabled])
		      fi
                  fi,
		  USING_DIST_MSI=true
		  AC_MSG_NOTICE([dist-msi support enabled]))
else
    AC_MSG_NOTICE([setting msi file... not set])
    AC_MSG_ERROR([a file must be specified when addind msi support])
fi
AM_CONDITIONAL([ax_dist_msi_enabled],[test "x$USING_DIST_MSI" = "xtrue"])
AX_ADD_AM_MACRO_STATIC([

if ax_dist_msi_enabled
msi dist-msi: \$(top_builddir)/\$(PACKAGE)-\$(VERSION).msi

\$(top_builddir)/\$(PACKAGE)-\$(VERSION).msi: \$(top_builddir)/\$(MSI_SETUP_FILE)
	@cp -f \"\$(top_builddir)/\$(MSI_SETUP_FILE)\" \"${AX_DOLLAR}@\"

])
AM_CONDITIONAL([ax_dist_msi_have_install_files],[test "x$AX_HAVE_INSTALL_FILES" = "xtrue"])
AX_ADD_AM_MACRO_STATIC([

if ax_dist_msi_have_install_files
\$(top_builddir)/\$(MSI_SETUP_FILE): \$(top_builddir)/install_files
	@echo \"the msi file (\$(MSI_SETUP_FILE)) must be (re)created\"; \\
	echo \"by building it with VC++\"; \\
	exit -1
else
\$(top_builddir)/\$(MSI_SETUP_FILE): msi_up_to_date_notice
	@if test ! -f \"\$(top_builddir)/\$(MSI_SETUP_FILE)\"; then \\
	    echo \"the msi file (\$(MSI_SETUP_FILE)) must be created\"; \\
	    echo \"by building it with VC++\"; \\
	    exit -1; \\
        fi

msi_up_to_date_notice:
	@if test -f \"\$(top_builddir)/\$(MSI_SETUP_FILE)\"; then \\
	    echo \"Warning: Dependency tracking cannot be enabled\"; \\
	    echo \"Warning: the msi file (\$(MSI_SETUP_FILE))\"; \\
	    echo \"Warning: Please make sure it is up to date.\"; \\
	    exit -1; \\
        fi
endif # ax_dist_msi_have_install_files
])

AM_CONDITIONAL([ax_dist_msi_using_ax_extra_dist],[test "x$USING_AX_EXTRA_DIST" != "x"])
AX_ADD_AM_MACRO_STATIC([

if ax_dist_msi_using_ax_extra_dist
EXTRA_BIN_DISTS += msi
endif # ax_dist_msi_using_ax_extra_dist

])

AM_CONDITIONAL([ax_dist_msi_using_ax_upload],[test "x$USING_AX_UPLOAD" != "x"])
AX_ADD_AM_MACRO_STATIC([

if ax_dist_msi_using_ax_upload
UPLOAD_BIN += upload-msi
UPLOAD_TARGETS += {msi=>\$(PACKAGE)-\$(VERSION).msi}
endif # ax_dist_msi_using_ax_upload

endif # ax_dist_msi_enabled

])
])

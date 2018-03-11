# ===========================================================================
#   https://www.gnu.org/software/autoconf-archive/ax_pkg_check_modules.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PKG_CHECK_MODULES(PREFIX, PUBLIC-MODULES, PRIVATE-MODULES, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [PUBLIC-VARIABLE], [PRIVATE-VARIABLE])
#
# DESCRIPTION
#
#   A wrapper around PKG_CHECK_MODULES which splits the list of modules into
#   public and private dependencies, and produces two variables listing the
#   dependencies across all invocations of AX_PKG_CHECK_MODULES. These two
#   variables are exposed via AC_SUBST, and should be used in a pkg-config
#   file as the substituted values for Requires and Requires.private.
#
#   The PREFIX, PUBLIC-MODULES and PRIVATE-MODULES arguments should be
#   specified as for PKG_CHECK_MODULES, with the concatenation of
#   PUBLIC-MODULES and PRIVATE-MODULES equaling the LIST-OF-MODULES from
#   PKG_CHECK_MODULES.  The ACTION-IF-FOUND and ACTION-IF-NOT-FOUND
#   arguments are optional, and should also be specified as for
#   PKG_CHECK_MODULES.  ACTION-IF-FOUND is evaluated if the full
#   LIST-OF-MODULES is found; ACTION-IF-NOT-FOUND similarly.
#
#   PUBLIC-VARIABLE defaults to AX_PACKAGE_REQUIRES, and PRIVATE-VARIABLE
#   defaults to AX_PACKAGE_REQUIRES_PRIVATE.  Both variables are AC_SUBST-ed
#   by this macro.
#
#   For example:
#
#     AX_PKG_CHECK_MODULES([GLIB],[glib-2.0 gio-2.0],[gthread-2.0])
#     AX_PKG_CHECK_MODULES([DBUS],[],[dbus-glib-1 >= 0.98 dbus-1])
#
#   results in the substitutions:
#
#     AX_PACKAGE_REQUIRES="glib-2.0 gio-2.0"
#     AX_PACKAGE_REQUIRES_PRIVATE="gthread-2.0 dbus-glib-1 >= 0.98 dbus-1"
#
#   and can be used with a template pkg-config file (.pc.in) using:
#
#     Requires: @AX_PACKAGE_REQUIRES@
#     Requires.private: @AX_PACKAGE_REQUIRES_PRIVATE@
#
# LICENSE
#
#   Copyright (c) 2014 Philip Withnall <philip@tecnocode.co.uk>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.  This file is offered as-is, without any
#   warranty.

#serial 4

AC_DEFUN([AX_PKG_CHECK_MODULES],[
    m4_define([ax_package_requires],
              [m4_default_quoted([$6],[AX_PACKAGE_REQUIRES])])
    m4_define([ax_package_requires_private],
              [m4_default_quoted([$7],[AX_PACKAGE_REQUIRES_PRIVATE])])

    ax_package_requires="$[]ax_package_requires m4_normalize($2)"
    ax_package_requires_private="$[]ax_package_requires_private m4_normalize($3)"

    PKG_CHECK_MODULES([$1],[$2 $3],[$4],[$5])

    # Substitute output.
    AC_SUBST(ax_package_requires)
    AC_SUBST(ax_package_requires_private)
])dnl AX_PKG_CHECK_MODULES

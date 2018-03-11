# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_prog_splint.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_SPLINT([AX_SPLINTFLAGS])
#
# DESCRIPTION
#
#   Check for program splint, the static C code checking tool. The splint
#   URL is given by http://www.splint.org. This macro should be use together
#   with automake.
#
#   Enables following environment variables:
#
#     SPLINT
#     SPLINTFLAGS
#
#   and AX_SPLINTFLAGS is given by AC_SUBST. If AX_SPLINTFLAGS is not given
#   by AX_PROG_SPLINT it defaults to "-weak".
#
#   Enables the following make target:
#
#     splint-check
#
#   which runs splint per PROGRAMS and LIBRARIES. Output from splint run is
#   collected in file ***_splint.log where *** is given by the PROGRAMS or
#   LIBRARIES name.
#
#   The following line is required in Makefile.am:
#
#     include aminclude_static.am
#
# LICENSE
#
#   Copyright (c) 2011 Henrik Uhrenholt
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_PROG_SPLINT],
[
AC_REQUIRE([AX_ADD_AM_MACRO_STATIC])
AC_ARG_VAR([SPLINT], [splint executable])
AC_ARG_VAR([SPLINTFLAGS], [splint flags])
if test "x$SPLINT" = "x"; then
   AC_CHECK_PROGS([SPLINT], [splint])
fi
if test "x$SPLINT" != "x"; then
   ax_splintflags=$1
   if test "x$1" = "x"; then
      ax_splintflags=-weak
   fi
   AC_SUBST([AX_SPLINTFLAGS], [$ax_splintflags])
   ax_prog_splint_enable=yes
else
   AC_MSG_WARN([splint support disabled])
   ax_prog_splint_enable=no
fi
AM_CONDITIONAL([ax_prog_splint_enable], [test x"$ax_prog_splint_enable" = x"yes"])

AX_ADD_AM_MACRO_STATIC([
if ax_prog_splint_enable

define splint_rules
\$(1)_splint.log: \$${AX_DOLLAR}(\$(1)_OBJECTS)
	-\$(SPLINT) \$(AX_SPLINTFLAGS) \$(SPLINTFLAGS)  \$(AM_SPLINTFLAGS) \$(DEFAULT_INCLUDES) \$(AM_CPPFLAGS) +error-stream-stdout +warning-stream-stdout \$${AX_DOLLAR}(addprefix \$(srcdir)/,\$${AX_DOLLAR}(\$(1)_SOURCES)) > \$${AX_DOLLAR}@
endef


SPLINT_BIN=\$(subst /,_,\$(PROGRAMS:\$(EXEEXT)=))
SPLINT_LIB=\$(subst /,_,\$(LIBRARIES:.a=_a))
SPLINT_LTLIB=\$(subst /,_,\$(LTLIBRARIES:.la=_la))
SPLINTFILES=\$(addsuffix _splint.log,\$(SPLINT_LIB) \$(SPLINT_BIN) \$(SPLINT_LTLIB))

splint-check: all \$(SPLINTFILES)

\$(foreach bin, \$(SPLINT_BIN) \$(SPLINT_LIB) \$(SPLINT_LTLIB),\$(eval \$(call splint_rules,\$(bin))))




endif

.PHONY: clean-local-splint distclean-local-splint
clean-local: clean-local-splint
clean-local-splint:
	-test -z \"\$(SPLINTFILES)\" || rm -f \$(SPLINTFILES)

distclean-local: distclean-local-splint
distclean-local-splint: clean-local-splint
])
])
